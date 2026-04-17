#include "Server.hpp"
#include "Client.hpp"
#include "ACommand.hpp"

#include <iostream>
#include <unistd.h>

void Server::shutdown()
{
	deleteCommandMap();
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
