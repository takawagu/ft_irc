#include "Server.hpp"
#include "Commands.hpp"

#include <iostream>

volatile sig_atomic_t Server::_stop = 0;

void Server::signalHandler(int)
{
	_stop = 1;
}

Server::Server(const Config& config): _config(config), _listen_fd(-1)
{
	initCommandMap();
}

Server::~Server()
{
	shutdown();
}

void Server::run()
{
	setup();
	loop();
}
