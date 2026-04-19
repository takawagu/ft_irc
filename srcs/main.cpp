#include "Config.hpp"
#include "Server.hpp"

#include <iostream>

// int main(int argc, char** argv)
// {
//     if (argc != 3) 
//     {
//         std::cerr << "Usage: ircserv <port> <password>" << std::endl;
//         return 1;
//     }

// 	try
// 	{
// 		Config config(argv[1], argv[2]);
// 		Server server(config);
// 		server.run();
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::cerr << "Error: " << e.what() << std::endl;
// 		return 1;
// 	}
// 	return 0;
// }

#include "Pass.hpp"

int main(void){
	std::cout << "tochi test" << std::endl;
	ACommand* cmd = new Pass();
	Config config("6667", "password");
	Server server(config);
	Client* client = nullptr;
	cmd->execute(server,*client,0,"Pass password\r\n");
}
