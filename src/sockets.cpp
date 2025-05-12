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
void extractPort(serverDetails& server){
    //first find the first :// (since : may be used 2 times if port contained)
    size_t pos = server.url.find("://");
    if(pos != std::string::npos) {
        pos += 3;
    }


    //endPos is where the port number starts
    size_t endPos = server.url.find(":",pos);
    if(endPos == std::string::npos){
        //no port found so determine whether to use http or https 
        if(server.url.find("https") != std::string::npos){
            server.port = HTTPS;
            return;
        }else{
            server.port = HTTP;
            return;
        }
    }

    //port found so extract it
    size_t pathPos = server.url.find("/",endPos);
    if(pathPos == std::string::npos){
        pathPos = server.url.length();
    }

    //extract the port
    server.port = std::stoi(server.url.substr(endPos + 1,pathPos - endPos-1));
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
        extractPort(server);//extract the port of each server
    }
}

int socketv4(const serverDetails& server){
    sockaddr_in client;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    if(sockfd == SOCKET_ERROR){
        std::cout << server.formatted_url << " : ";
        perror("Error creating socket for ipv4");
        return SOCKET_ERROR;
    }

    //first ensure to clear client
    memset(&client, 0, sizeof(client));
    //connect to servers IPv4
    client.sin_family = AF_INET;
    client.sin_port   = htons(server.port);
    if(inet_pton(AF_INET,server.ipv4.c_str(),&client.sin_addr) <= 0){
        std::cout << server.formatted_url << " : ";
        perror("error connecting to server through IPv4");
        return SOCKET_ERROR;
    } 


    //connect to server 
    if(connect(sockfd, (struct sockaddr*)&client, sizeof(client)) < 0){
        std::cout << server.formatted_url << " : ";
        perror("connection failed");
        return SOCKET_ERROR;
    }


    return sockfd; //Return the created socket file descriptor
}



int socketv6(const serverDetails& server) {
    sockaddr_in6 client;
    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);  // Create an IPv6 socket

    if (sockfd == SOCKET_ERROR) {
        std::cout << server.formatted_url << " : ";
        perror("Error creating socket for IPv6");
        return SOCKET_ERROR;
    }

    memset(&client, 0, sizeof(client));  // Ensure client is cleared
    client.sin6_family = AF_INET6;  
    
    client.sin6_port = htons(server.port); 

    // Convert IPv6 address string to binary form 
    if (inet_pton(AF_INET6, server.ipv6.c_str(), &client.sin6_addr) <= 0) {
        std::cout << server.formatted_url << " : ";
        perror("error connecting to server through IPv6");
        return SOCKET_ERROR;
    }

    // Attempt to connect to the server via IPv6
    if (connect(sockfd, (struct sockaddr*)&client, sizeof(client)) < 0) {
        std::cout << server.formatted_url << " : ";
        perror("Connection failed");
        return SOCKET_ERROR;
    }

    return sockfd;  // Return the socket file descriptor
}
