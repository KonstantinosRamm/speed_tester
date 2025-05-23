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
    std::promise<void> promise;
    std::future<void> future = promise.get_future();

    ix::WebSocket ws;
    //required headers from server to not get error 400
    //
    ws.setExtraHeaders({
        {"Sec-WebSocket-Protocol", "net.measurementlab.ndt.v7"},
        {"User-Agent", "ndt7-cpp-client"}
    });
    
    //retrieve the proper path .Required path ---> scheme/domain/access_token
    ws.setUrl(RetrieveBaseUrl(servers.at(0).download_wss)); //retrieve the proper format of the connection url

    ws.setOnMessageCallback([&](const ix::WebSocketMessagePtr& msg) {
        if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "Connected successfully.\n";
            ws.close();
        }
        else if (msg->type == ix::WebSocketMessageType::Close) {
            std::cout << "Connection closed.\n";
            promise.set_value();
        }
        else if (msg->type == ix::WebSocketMessageType::Error) {
            std::cerr << "Error: " << msg->errorInfo.reason << "\n";
            promise.set_value();
        }
    });

    ws.start();
    future.get();
}
