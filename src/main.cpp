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

   std::cout << "____________________PING____________________" << std::endl;
   FindPing(servers);//ping servers to find the best one 
   sortServers(servers);//sort servers based on ping


   for (auto& server : servers){
      std::cout << server.machine << " ping : " << server.ping << std::endl;
   }

   return 0;
}
