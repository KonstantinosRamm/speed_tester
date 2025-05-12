#include "sockets.hpp"
#include "curlfunctions.hpp"


void resolveServers(std::vector<serverDetails>& servers){
    //iterate through the vector to resolve all the available servers
    for (auto &server : servers){
        struct addrinfo hints, *result;
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_UNSPEC;//for use with both IPv4 IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP protocol

        int res = getaddrinfo(server.formatted_url.c_str(),nullptr,&hints,&result);

        if(res != 0){
            std::cerr << "Failed to resolve URL: " << server.formatted_url << "Error :" << gai_strerror(res) << std::endl;
            continue;
        }

        //loop through the resolved adresses and grab the first Ipv4 and Ipv6
        for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next){
            //IPv4
            if(ptr->ai_family == AF_INET && server.ipv4.empty()){
                char  ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &((struct sockaddr_in*)ptr->ai_addr)->sin_addr, ip, sizeof(ip));
                server.ipv4 = ip;
            }
            //IPv6
            else if(ptr->ai_family == AF_INET6 && server.ipv6.empty()){
                char ip[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &((struct sockaddr_in6*)ptr->ai_addr)->sin6_addr, ip, sizeof(ip));
                server.ipv6 = ip;
            }


            //if both IPv4 and IPv6 found stop processing 
            if(!server.ipv4.empty() && !server.ipv6.empty()){
                break;
            }
        }

        //free result after usage
        freeaddrinfo(result);
    }
}


void formatServersURL(std::vector<serverDetails>& servers){
    //extract the part of the url between the :// and :
    for(auto &server : servers){
        size_t pos = server.url.find("://");
        if(pos != std::string::npos){
            pos += 3;// skip ://
        }

        size_t endPos = server.url.find(":",pos);
        if(endPos == std::string::npos){
            endPos = server.url.length(); // no path so take everything after ://
        }
        //store the formatted url inside the servers details 
        server.formatted_url = server.url.substr(pos,endPos - pos);
    }
}

int create_socketv4(const serverDetails& server,sockaddr_in &client){
    
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    if(sockfd == SOCKET_ERROR){
        std::cout << server.formatted_url << " : ";
        perror("Error creating socket for ipv4");
    }
}