#include <curl/curl.h>//external library (libcurl)
#include <iostream>
#include <vector>
#include <string.h>
#include <unistd.h>
#include "curlfunctions.hpp"
#include "json.hpp"





int main(){
    PersonalInfo personal_info;

    //init curl globaly for full use of the library
    CURLcode globalInitResult = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (globalInitResult != CURLE_OK) {
        std::cerr << "libcurl initialization failed!" << std::endl;
        return 1;
    }

    // create the curl handle 
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "CURL initialization failed!" << std::endl;
        curl_global_cleanup(); // cleanup global settings before exiting
        return 1;
    }


    
    //vector of all available servers based on geolocation
    std::vector<serverDetails> servers;

    bool res = requestPersonalInfo(IP_INFO_URL,personal_info);
    if(!res){
        std::cout << "error retrieving personal data" << std::endl;
        return 1;
    }



    //apply curl request to retrieve available servers for speedtesting
    res = requestAvailableServers(SERVERS_URL,servers);
    if(!res){
        std::cout << "error retrieving available servers" << std::endl;
        return 1;
    }


    //extract the url of the first available server
    if(prepareSpeedtestCurl(servers,curl)){
        std::cout << "curl handle setup complete" << std::endl;
    }else{
        //in case no server found clear curl handle and exit
        curl_easy_cleanup(curl);
        return 1;
    }
    //request users personal info(just for display)
    printPersonalData(personal_info);   

    //apply ping
    //apply download 
    //apply upload

    curl_global_cleanup();
    curl_easy_cleanup(curl);
    return 0;
}