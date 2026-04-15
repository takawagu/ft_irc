#include "Server.hpp"

Server::Server(const Config& config): _config(config)
{
}

Server::~Server() {}

void Server::run()
{
	std::cout << "port     = " << _config.port() << std::endl;
	std::cout << "password = " << _config.password() << std::endl;
}
