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
   FindPing(servers);

   return 0;
}
