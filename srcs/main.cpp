#include "Config.hpp"
#include "Server.hpp"

#include <iostream>

void commandTest(int argc, char** argv);

static void ircserv(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ircserv <port> <password>" << std::endl;
		return;
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
	}
}

int main(int argc, char** argv)
{
	ircserv(argc, argv);
	commandTest(argc,argv);
	return 0;
}
