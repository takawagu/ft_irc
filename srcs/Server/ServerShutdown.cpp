#include "Server.hpp"
#include "Client.hpp"
#include "ACommand.hpp"
#include "Channel.hpp"

#include <iostream>
#include <unistd.h>

void Server::closeAllChannels()
{
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	_channels.clear();
}

void Server::shutdown()
{
	deleteCommandMap();
	closeAllChannels();
	closeAllClients();
	closeListenFd();

	std::cout << "Server shut down." << std::endl;
}

void Server::closeAllClients()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	_pfds.clear();
}

void Server::closeListenFd()
{
	if (_listen_fd >= 0)
	{
		close(_listen_fd);
		_listen_fd = -1;
	}
}
