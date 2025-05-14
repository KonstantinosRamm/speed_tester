#include "curlfunctions.hpp"
#include <fstream>  // For file handling
#include <iostream> // For logging purposes (optional)

size_t writedata(void *ptr, size_t size, size_t nmemb, void *data) {
    std::string *response = (std::string*)data;
    size_t totalSize = size * nmemb;
    response->append((char*)ptr, totalSize);
    return totalSize;
}

/**
 * @brief Sends a request to the specified server and writes the response to a file.
 * 
 * This function sends a request to the given URL using CURL, stores the response in a string,
 * and then writes the response to a JSON file (SERVER_FILE). It returns a success status.
 * 
 * @param url The URL of the server to send the request to.
 * 
 * @return `true` if the request was successful and data was written to the file, `false` otherwise.
 */
bool requestPersonalInfo(const std::string& url, PersonalInfo& personal_info){
    // Initialize the curl handle
    CURL *handle = curl_easy_init();
    if (!handle) {
        return false;
    }

    // Setup CURL flags
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle,CURLOPT_TIMEOUT,5L); // Timeout after 5 seconds
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writedata);

    // Variable to store the response data
    std::string response_data;

    // Set the pointer to the response string for storing the output
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    CURLcode res = curl_easy_perform(handle);
    if (res != CURLE_OK) {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(handle);
        return false;
    }

    //retrieve data from string to json
    //if any field is empty or null it will return null to avoid errors
    try{
        nlohmann::json json_parser = nlohmann::json::parse(response_data);
        personal_info.ip = json_parser["ip"];
        personal_info.city = json_parser["city"];
        personal_info.country = json_parser["country"];
        personal_info.loc = json_parser["loc"];

    }catch (const nlohmann::json::exception& e){
        std::cerr << "JSON parsing error : " << e.what() << std::endl;
        curl_easy_cleanup(handle);
        return false;
    }

    // Cleanup the CURL object
    curl_easy_cleanup(handle);

    // Return true indicating success
    return true;
}

void printPersonalData(const PersonalInfo &personal_info){
    std::cout << "PERSONAL INFO : " << std::endl;
    std::cout << "ip       : " << personal_info.ip << std::endl;
    std::cout << "city     : " << personal_info.city << std::endl;
    std::cout << "country  : " << personal_info.country << std::endl;
    std::cout << "location : " << personal_info.loc << std::endl;
}



void sortServers(std::vector<serverDetails>& servers){
    // Use std::sort with a custom comparator to sort based on distance
    std::sort(servers.begin(),servers.end(),[](const serverDetails&  a,const serverDetails& b){
        return a.ping < b.ping;
    });
}



void typeCastJson(serverDetails& s, const nlohmann::json& server) {
    // Parse fields safely — some might be missing, or stored as strings

    if (server.contains("url"))
        s.url = server["url"];

    if (server.contains("lat"))
        s.lat = std::stod(server["lat"].get<std::string>());

    if (server.contains("lon"))
        s.lon = std::stod(server["lon"].get<std::string>());

    if (server.contains("distance"))
        s.distance = server["distance"].get<int>();

    if (server.contains("name"))
        s.name = server["name"];

    if (server.contains("country"))
        s.country = server["country"];

    if (server.contains("cc"))
        s.cc = server["cc"];

    if (server.contains("sponsor"))
        s.sponsor = server["sponsor"];

    if (server.contains("id"))
        s.id = std::stoi(server["id"].get<std::string>());

    if (server.contains("preferred"))
        s.preferred = (server["preferred"].get<int>() != 0);

    if (server.contains("https_functional"))
        s.https_functional = (server["https_functional"].get<int>() != 0);

    if (server.contains("host"))
        s.host = server["host"];

    if (server.contains("force_ping_select"))
        s.force_ping_select = (server["force_ping_select"].get<int>() != 0);
}


//request servers from speedtest api
bool requestAvailableServers(const std::string& url,std::vector<serverDetails>& servers_vector){
    CURL *handle;
    //resolve urls into ip adresses
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // Initialize the curl handle
    handle = curl_easy_init();
    if (!handle) {
        std::cerr << "Failed to initialize CURL handle" << std::endl;
        return false;
    }

    // Setup CURL flags
    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L); // Timeout after 5 seconds
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writedata);
    

    // Variable to store the response data
    std::string response_data;

    // Set the pointer to the response string for storing the output
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    CURLcode res = curl_easy_perform(handle);
    if (res != CURLE_OK) {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(handle);
        return false;
    }

    //store data in json format to retrieve them easier and format them 
    nlohmann::json server_json = nlohmann::json::parse(response_data);

    //loop through the json array and fill the vector 
    for (const auto& server : server_json){
        serverDetails s;



        // Add the server to the vector
        typeCastJson(s,server);

        servers_vector.push_back(s);
        
    }
    // Cleanup the CURL object
    curl_easy_cleanup(handle);

    // Return true indicating success
    return true;
}



serverDetails prepareSpeedtestCurl(const std::vector<serverDetails>& servers,CURL* curl){
    //iterate and find the first available server
    for(const auto &server : servers){
        //reset between each iteration
        curl_easy_reset(curl);
        curl_easy_setopt(curl,CURLOPT_URL,server.url.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Timeout after 5 seconds
        CURLcode res = curl_easy_perform(curl);
        //return true if a ping succeed
        if (res == CURLE_OK){
            return server;
        }
    }
    //return the default server constructor instead 
    return serverDetails();
}


std::string formatServersURL(const std::string& url){
    //extract the part of the url between the :// and :
    
    size_t pos = url.find("://");
    if(pos != std::string::npos){
        pos += 3;// skip ://
    }

    size_t endPos = url.find(":",pos);
    if(endPos == std::string::npos){
        endPos = url.length(); // no path so take everything after ://
    }
    //store the formatted url inside the servers details 
    std::string formatted_url = url.substr(pos,endPos - pos);
    //return the formatted url 
    return formatted_url;
    
    
}


void findPing(CURL* curl,std::vector<serverDetails>& servers){
    for(auto &server : servers){
        //reset curl before the ping(RTT) operation and set curl url to the current server url
        curl_easy_reset(curl);
        curl_easy_setopt(curl,CURLOPT_URL,server.url.c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // don't download body
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Timeout after 5 seconds
        int ping = INFINITE_PING;
        //check for sure that the curl handle is initialized
        if(curl){
            auto start_time = std::chrono::system_clock::now();
            //check if ping operation succeeded
            CURLcode res = curl_easy_perform(curl);
            if(res == CURLE_OK){
                auto end_time = std::chrono::system_clock::now();

                //calculate duration in ms 
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time).count();
                ping = static_cast<int>(duration);
            }else{
                std::cerr << "CURL error on ping operation: " << curl_easy_strerror(res) << "\n";
                
            }
        }
        server.ping = ping;
    }
}









