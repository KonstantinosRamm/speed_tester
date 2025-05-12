#include <curl/curl.h>//external library (libcurl)
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <cerrno>
#include <cstdlib>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "curlfunctions.hpp"
#include "json.hpp"
#include "sockets.hpp"




int main(){
    PersonalInfo personal_info;
    //vector of all available servers based on geolocation
    std::vector<serverDetails> servers;
    bool res = requestPersonalInfo(IP_INFO_URL,personal_info);
    if(!res){
        std::cout << "error retrieving personal data" << std::endl;
        return 1;
    }
    //request users personal info(just for display)
    printPersonalData(personal_info);


    //apply curl request to retrieve available servers for speedtesting
    res = requestAvailableServers(SERVERS_URL,servers);
    if(!res){
        std::cout << "error retrieving available servers" << std::endl;
        return 1;
    }

    //format all urls to be able to resolve them
    formatServersURL(servers);
    //resolve all adresses url into ip adresses
    resolveServers(servers);



    

    //find and available server
    for(const auto & server: servers){
        //implement  the socket for IPv4
        int sockfd4 = SOCKET_ERROR;

        int sockfd6 = SOCKET_ERROR;



        //create socket and connect to server On IPv4 and IPv6

        sockfd4 = socketv4(server);
        sockfd6 = socketv6(server);

        //implement speed testing for IPv4 socket
        if(sockfd4 != SOCKET_ERROR){
            std::cout << "connected to : " << server.formatted_url << "IPv4" << std::endl;
            close(sockfd4);
            return 0;
        }
        //implement speed testing for IPv6 socket
        if(sockfd6 != SOCKET_ERROR){
            std::cout << "connected to : " << server.formatted_url << "IPv6" << std::endl;
            close(sockfd6);
            return 0;
        }
    }   





    //implement the speed test now ping-download-upload

    return 0;
}