#include "servers.hpp"

//overloaded function to write data retrieved into a string
static size_t writeServers(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}




//find all available servers and store them in the servers argument
bool findAvailableServers(std::vector<ServerInfo> & servers){
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