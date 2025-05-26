#include "json.hpp"
#include "servers.hpp"
#include <iostream>
#include <string>
#include <vector>



int main(void){
   std::vector<ServerInfo> servers;//vector of all available servers found

   bool result = FindAvailableServers(servers);
   
   if(!result){
      std::cerr << "could not complete operation : findAvailableServers" << std::endl;
      return 1;
   }

   std::cout << "____________________PING LOGS____________________" << std::endl;
   FindPing(servers);//ping servers to find the best one 
   sortServers(servers);//sort servers based on ping

   std::cout << "____________________UPLOAD TEST____________________" << std::endl;
   //iterate all the servers and find the first one with a successful connection 
   for (auto &server : servers){
      //if the first test succeed then break the program
      if(uploadTest(server)){
         break;
      }
   }
   

   return 0;
}
