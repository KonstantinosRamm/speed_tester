#pragma once 
#include "json.hpp"
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ixwebsocket/IXWebSocket.h>
#include <chrono>
#include <future>
#include <cstdlib>
 
constexpr int default_ping = -1;//default value before the ping calculated for each server
constexpr const char* MLAB_API = "https://locate.measurementlab.net/v2/nearest/ndt/ndt7"; //mlab locate v2 API to retrieve nearest servers available for testing
//array fields for use with the MLAB Locate v2 API
constexpr const char* url_ws_download = "ws:///ndt/v7/download";
constexpr const char* url_ws_upload = "ws:///ndt/v7/upload";
constexpr const char* url_wss_download = "wss:///ndt/v7/download";
constexpr const char* url_wss_upload = "wss:///ndt/v7/upload";
/**
 * @brief struct used to represent the servers information retrieved from the mlab locate v2 api
 */
struct ServerInfo{
    /**
     * @brief the name of the machine
     */
    std::string machine = "";
    /**
     * @brief host name
     */
    std::string hostname = "";
    /**
     * @brief city of the located server
     */
    std::string city = "";
    /**
     * @brief country of the located server
     */
    std::string country = "";
    /**
     * @brief download url endpoint without encryption
     */
    std::string download_ws = "";
    /**
     * @brief upload url endpoint without encryption
     */
    std::string upload_ws = "";
    /**
     * @brief download url endpoint with encryption
     */
    std::string download_wss = "";
    /**
     * @brief upload url endpoint with encryption
     */
    std::string upload_wss = "";
    /**
     * @brief ping of the current server
     */
    int ping = default_ping;

};

/**
 * @brief the function is used in order to find all available servers for speed testing based on geolocation
 * utilizing the MLAB Locate API v2.It uses libcurl to make the http request to the MLAB API and retrieve the available servers
 * and json.hpp to extract the data into json format 
 * @param servers reference to the vector of struct serverInfo that is going to be used to store the servers info
 * @return true if operation succeed else false if it failed at any point 
 */
bool FindAvailableServers(std::vector<ServerInfo> & servers);


/**
 * @brief takes as argument the vector of all available servers found and calculates the ping for each one of them
 * @param servers reference to the vector of struct serverInfo that is going to be used to store the servers info
 * 
 */

void FindPing(std::vector<ServerInfo> & servers);


/**
 * @brief takes the url of the server and extracts the base url from it
 * @param string that contains the whole url of the server
 * @return the base endpoint of the server
 */
std::string RetrieveBaseUrl(const std::string& url);


/**
 * @brief takes the url of the server and extracts the access token
 * @param string that contains the whole url of the server
 * @return the access token
 */

 std::string RetrieveAccessToken(const std::string& url);
