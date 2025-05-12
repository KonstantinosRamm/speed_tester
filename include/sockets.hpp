#pragma once
#include <iostream>          // duh
#include <cstring>           // for memset, strerror, etc.
#include <string>
#include <sys/types.h>       // for data types
#include <sys/socket.h>      // for socket functions
#include <netdb.h>           // for getaddrinfo, struct addrinfo
#include <arpa/inet.h>       // for inet_ntop, inet_pton
#include <unistd.h>          // for close()
#include <vector>
#include <cerrno>
#include <cstdlib>
#include <string.h>
#include <netinet/in.h>
#include "curlfunctions.hpp" // for serverDetails struct



#define SOCKET_ERROR -1
#define HTTPS 443
#define HTTP 80

#define SOCKET_ERROR -1


/*
    resolve the url servers into ipv4 and ipv6
*/
void resolveServers(std::vector<serverDetails>& servers);


void formatServersURL(std::vector<serverDetails>& servers);


int socketv4(const serverDetails& server);
int socketv6(const serverDetails& server);
void extractPort(serverDetails & server);//find if http or https used based url

