#include "Config.hpp"
#include "Server.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 3) 
    {
        std::cerr << "Usage: ircserv <port> <password>" << std::endl;
        return 1;
    }

	try
	{
		Config config(argv[1], argv[2]);
		Server server(config);
		server.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
