#pragma once
#include <curl/curl.h>
#include "json.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

#define IP_INFO_URL "https://ipinfo.io/json"  // URL used to extract public IP, geolocation, city to determine the server to connect to.
#define SERVER_FILE "server_data.json"        // The path to the JSON file where data would be saved (currently unused).
#define SERVERS_URL "https://www.speedtest.net/api/js/servers" // URL used to retrieve the available speed testing servers based on ip geolocation



/**
 * @brief struct used to store personal info of the user
 */
struct PersonalInfo {
    std::string ip;      // Public IP address of the device.
    std::string city;    // City associated with the IP address.
    std::string country; // Country associated with the IP address.
    std::string loc;     // Geolocation coordinates (latitude and longitude).
};


/**
 * @brief struct used to store server details that retrieved from the speed testing api
 */
struct serverDetails{
    std::string url = "";               //full url of the server    
    double lat = 0.0;                   //latitude               
    double lon = 0.0;                   //longtitude             
    int distance = 0;                   //distance from client       
    std::string name = "";              //servers name
    std::string country = "";           //country
    std::string cc = "";                //iso country code 
    std::string sponsor = "";           //sponsor of the server           
    int id = 0;                         //servers id           
    bool preferred = false;                    
    bool https_functional = false;         
    std::string host = "";             //host computer    
    bool force_ping_select = false;  
};

/**
 * @brief Callback function used with CURLOPT_WRITEFUNCTION to handle data received during the request.
 * 
 * This function is invoked by libcurl to handle the data received from the server. The response data 
 * is appended to the memory location specified by the `data` parameter (typically a `std::string`).
 *
 * @param ptr A pointer to the received data.
 * @param size The size of each individual data element (e.g., 1 byte).
 * @param nmemb The number of data elements received (e.g., number of bytes in the response).
 * @param data A pointer to the destination location (typically a `std::string`) where the data will be written.
 * 
 * @return The total number of bytes written. If an error occurs, a non-zero value should be returned.
 * 
 * This function is used by libcurl during HTTP requests to store or process the server's response.
 */
size_t writedata(void *ptr, size_t size, size_t nmemb, void *data);


/**
 * @brief Sends an HTTP request to the specified URL and stores the response in a `PersonalInfo` struct.
 * 
 * This function sends an HTTP request to the specified URL using the CURL library, retrieves the response, 
 * parses it as JSON, and stores the relevant data (IP, city, country, and location) directly into the provided 
 * `PersonalInfo` struct. It does not write the data to a file; instead, the struct is populated with the 
 * extracted values.
 * 
 * @param url The URL of the server to send the request to. This should be a valid URL (e.g., "https://example.com").
 * @param personal_info A reference to the `PersonalInfo` struct where the extracted data will be stored.
 * 
 * @return `true` if the request was successful and the data was parsed and stored correctly in the `personal_info` struct, 
 *         `false` if there was an error.
 */
bool requestPersonalInfo(const std::string& url, PersonalInfo& personal_info);

/**
 * @brief print users personal info (ip,country,location,city)
 * @param a reference to the personal_info struct
 */
void printPersonalData(const PersonalInfo &personal_info);


/**
 * @brief function to request all available servers based on geolocation
 * @brief the API from which to request the geolocation data
 * @returns true if operation succeeded else false
 * 
 */
bool requestAvailableServers(const std::string& url, std::vector<serverDetails>& servers_vector);


/**
 *@brief function to sort the servers vector based on distance
 */

void sortServers(std::vector<serverDetails>& servers);


/**
 * @brief typecast each field from json to the appropriate format
 * @param s the current servers fields to be processed
 * @param server json file reference to parse data from
 */
void typeCastJson(serverDetails& s,const nlohmann::json& server);


/**
 * @brief takes as argument the vector with all available servers and finds the first available server by performing an http request through libcurl 
 * if it succeeds then this server is selected and returns true
 * @param servers the vector of available servers
 * @param curl the curl handle to be used for the http request 
 * @returns true if found a server else false
 */
bool prepareSpeedtestCurl(const std::vector<serverDetails>& servers,CURL* curl);


/**
 * @brief function used to calculate ping (RTT)
 * @param url the url of the server of the server to enstablish the connection
 */
int findPing(const std::string &url);








