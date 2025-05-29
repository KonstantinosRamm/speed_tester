#include "json.hpp"
#include "servers.hpp"
#include "conversions.hpp"
#include <iostream>
#include <string>
#include <vector>



int main(void){
   std::cout << std::fixed << std::setprecision(2) << std::endl;
   std::vector<ServerInfo> servers;//vector of all available servers found

   bool result = FindAvailableServers(servers);
   
   if(!result){
      std::cerr << "could not complete operation : findAvailableServers" << std::endl;
      return 1;
   }

   std::cout << "PING: " << std::endl;
   FindPing(servers);//ping servers to find the best one 
   sortServers(servers);//sort servers based on ping

   std::cout << "UPLOAD-DOWNLOAD: " << std::endl;
   //iterate all the servers and find the first one with a successful connection 
   bool upload_succeed =   false;
   bool download_succeed = false;
   for (auto &server : servers){
      //if the first upload to server succeed dont repeat the upload proccess
      if(uploadTest(server) && !upload_succeed){
         upload_succeed = true;
      }
      //if the first upload to server succeed dont repeat the upload proccess
      if(downloadTest(server) && !download_succeed){
         download_succeed = true;
      }
      //if both download and upload already succeed break the loop
      if(download_succeed && upload_succeed){
         break;
      }
   }
   return 0;
}
