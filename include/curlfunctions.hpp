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
    std::string ipv4 = "";
    std::string ipv6 = "";
    std::string url = "";     
    std::string formatted_url = "";           
    double lat = 0.0;                      
    double lon = 0.0;                      
    int distance = 0;                      
    std::string name = "";                 
    std::string country = "";              
    std::string cc = "";                   
    std::string sponsor = "";             
    int id = 0;                            
    bool preferred = false;                
    bool https_functional = false;         
    std::string host = "";                 
    bool force_ping_select = false;  
    int port;    

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
 * 
 * @example
 * PersonalInfo info;
 * bool success = request("https://ipinfo.io/json", info);
 * if (success) {
 *     std::cout << "IP: " << info.ip << std::endl;
 *     std::cout << "City: " << info.city << std::endl;
 *     std::cout << "Country: " << info.country << std::endl;
 *     std::cout << "Location: " << info.loc << std::endl;
 * }
 * 
 * @see curl_easy_init(), curl_easy_setopt(), curl_easy_perform(), curl_easy_cleanup(), nlohmann::json::parse()
 */
bool requestPersonalInfo(const std::string& url, PersonalInfo& personal_info);


/**
 * @brief print users personal info (ip,country,location,city)
 * @param a reference to the personal_info struct
 */
void printPersonalData(const PersonalInfo &personal_info);


/**temporary to request available servers based on geolocation */
bool requestAvailableServers(const std::string& url, std::vector<serverDetails>& servers_vector);


/**
 * sort servers_vector based on distance
 */

void sortServers(std::vector<serverDetails>& servers);


/**
 * typecast each field from json to the appropriate format
 */
void typeCastJson(serverDetails& s,const nlohmann::json& server);


// Helper function to safely get a value from the JSON and convert it
//template<typename T>
//T safe_get(const nlohmann::json& j, const std::string& key, const T& default_value = T());
// Declare the generic version first



