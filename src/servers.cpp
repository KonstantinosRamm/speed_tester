#include "servers.hpp"

//overloaded function to write data retrieved into a string
static size_t writeServers(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}


//find all available servers and store them in the servers argument
bool FindAvailableServers(std::vector<ServerInfo> & servers){
    //CURL handle to perform the http request
    CURL* handle = curl_easy_init();
    //check if curl handle initialized properly
    if(!handle){
        std::cerr << "[Error] initializeing curl handle to retrieve available servers " << std::endl;
        return false;
    }

    std::string response_string; // used to temporarily store the data fetched from api
    //set curl handle options
    curl_easy_setopt(handle,CURLOPT_URL,MLAB_API);
    curl_easy_setopt(handle,CURLOPT_WRITEFUNCTION,writeServers);
    curl_easy_setopt(handle,CURLOPT_WRITEDATA, &response_string);


    //perform the curl operations
    CURLcode result = curl_easy_perform(handle);
    //check if all operations succeed
    if( result != CURLE_OK){
        std::cerr << "[Error] curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
        curl_easy_cleanup(handle);
        return false;
    }
    //clean curl handle to avoid memory leaks
    curl_easy_cleanup(handle);

    //try to fetch data from the string to json format
    //and catch any possible exception
    //for more info about the MLAB Locate v2 API
    //https://www.measurementlab.net/develop/locate-v2/
    nlohmann::json results;

    try{
        results = nlohmann::json::parse(response_string);
    }catch (nlohmann::json::parse_error& e){
        std::cerr << "[Error] Failed to parse JSON: " << e.what() << std::endl;
        return false;
    }

    //check if the json contains any servers available

    if (!results.contains("results") || results["results"].empty()) {
        std::cerr << "[Info] No available servers found." << std::endl;
        return false;
    }



    //parse data to the vector
    for(const auto & server : results["results"]){
        //temporary server info object 
        ServerInfo s;

        s.machine = server.value("machine", "");
        s.hostname = server.value("hostname", "");
        s.city = server["location"].value("city", "");
        s.country = server["location"].value("country", "");
        s.download_ws = server["urls"].value(url_ws_download, "");
        s.upload_ws = server["urls"].value(url_ws_upload, "");
        s.download_wss = server["urls"].value(url_wss_download, "");
        s.upload_wss = server["urls"].value(url_wss_upload, "");


        //push the server to the server vector
        servers.push_back(s);

    }
    return true;
}


std::string RetrieveBaseUrl(const std::string& url){
    //wws://baseUrl?token
    //extract the base url with the access token 
    //in m-lab the ? is replaced with \u0026 to avoid encoding issues
    return url.substr(0,url.find("\u0026"));
}

std::string RetrieveAccessToken(const std::string& url){
    std::string token_key = "access_token=";
    size_t start = url.find(token_key);
    //return empty string if npos reached
    if(start == std::string::npos){
        return "";
    }
    //add an offset to the position of the string
    start += token_key.length();


    //find & which indicates the end of token key starting from offset at start index
    size_t end = url.find('&', start);

    if(end == std::string::npos){
        return url.substr(start);
    }


    //return the access token
    return url.substr(start,end - start);

    
}


void FindPing(std::vector<ServerInfo>& servers){
    
    for (auto& server : servers)
    {
        std::promise<void> promise;
        std::future<void> future = promise.get_future();

        std::chrono::steady_clock::time_point start_time;
        std::chrono::steady_clock::time_point end_time;

        ix::WebSocket wss;
        //set headers otherwise the server will respond with Error 400 or Error 401
        wss.setExtraHeaders({
            {"Sec-WebSocket-Protocol", "net.measurementlab.ndt.v7"},
            {"User-Agent", "ndt7-cpp-client"}
        });
    
        wss.setUrl(RetrieveBaseUrl(server.download_wss));


        //ping logic
        wss.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
            if (msg->type == ix::WebSocketMessageType::Open) {
                start_time = std::chrono::steady_clock::now();
                wss.ping("Ping");
            }
            else if (msg->type == ix::WebSocketMessageType::Pong){
                end_time = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
                server.ping = duration;
                wss.close();
            }
            else if (msg->type == ix::WebSocketMessageType::Close) {
                    promise.set_value();
            }
            else if (msg->type == ix::WebSocketMessageType::Error) {
                std::cerr << std::endl << "Error: " << msg->errorInfo.reason << "\n";
                wss.close();
                promise.set_value();
                
            }
        });

        wss.start();

        if (future.wait_for(std::chrono::seconds(5)) == std::future_status::timeout) {
            std::cerr << "Timeout on server: " << server.machine << "\n";
            wss.close();
        } else {
            future.get();
        }
    }
}

void sortServers(std::vector<ServerInfo>& servers) {
    std::sort(servers.begin(), servers.end(), [](const ServerInfo& a, const ServerInfo& b) {
        return a.ping < b.ping;
    });
}



bool uploadTest(const ServerInfo& server){
    if (server.ping == default_ping) return false;

    double Mbps = 0;
    //buffer of data to be send
    std::string buffer(chunk_size, 'A');

    //promise to avoid memory leaks since IXWebsocket is fully asynchronous
    std::promise<void> promise;
    auto future = promise.get_future();

    std::atomic<bool> promiseSet{false};

    //json object of last response(since only the last server response is important for us)
    nlohmann::json lastJson;


    ix::WebSocket wss;
    //headers required for connection to server
    wss.setExtraHeaders({
        {"Sec-WebSocket-Protocol", "net.measurementlab.ndt.v7"},
        {"User-Agent", "ndt7-cpp-client"}
    });
    wss.setUrl(RetrieveBaseUrl(server.upload_wss));

    wss.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            wss.sendBinary(buffer);
        }
        else if (msg->type == ix::WebSocketMessageType::Message) {
            try {//fields required from server response for upload testing
                auto json_msg = nlohmann::json::parse(msg->str);
                if (json_msg.contains("TCPInfo") && json_msg["TCPInfo"].contains("ElapsedTime") && json_msg["TCPInfo"].contains("BytesSent")) {
                    lastJson = json_msg;
                }
                wss.sendBinary(buffer);
            }
            catch (const std::exception& e) {
                std::cerr << "JSON error: " << e.what() << std::endl;
            }
        }//error and close 
        else if ((msg->type == ix::WebSocketMessageType::Close || msg->type == ix::WebSocketMessageType::Error) && !promiseSet.exchange(true)) {
            if (msg->type == ix::WebSocketMessageType::Error) {
                std::cerr << "Upload Error: " << msg->errorInfo.reason << std::endl;
                wss.close();
            }
            promise.set_value();
        }
    });

    wss.start();
    //NDT7 uses a stream between server and client for 10 seconds normally ,but we set a timeout of max_wait_upload to avoid infinite loops 
    if (future.wait_for(std::chrono::seconds(max_wait_upload)) == std::future_status::timeout) {
        std::cerr << "Upload test timeout on server: " << server.machine << std::endl;
        wss.close();
        return false;
    }

    try {
        size_t bytesSent = lastJson["TCPInfo"]["BytesReceived"];// parse bytes received from server
        double elapsedTime = lastJson["TCPInfo"]["ElapsedTime"]; // in microseconds
        double seconds = elapsedTime / 1'000'000.0; // divide to get seconds we will get almost 10 everytime since it uses NDT7
        double bits = bytesSent * 8.0; //calculate bits instead
        Mbps = bits / seconds / 1'000'000.0;//convert to megabit/s

        //TODO
        //implement function to convert mbps to gbps and kbps
        
        std::cout << "Server Location: " << server.city << " - " << server.country << std::endl;
        std::cout << "Speed : " << Mbps << " mbps" << std::endl;
        return true;
    } catch (...) {
        std::cerr << "Failed to extract results from upload test." << std::endl;
        return false;
    }
}









