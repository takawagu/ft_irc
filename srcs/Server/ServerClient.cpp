#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#include <iostream>
#include <unistd.h>

Client* Server::findClient(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return NULL;
	return it->second;
}

void Server::addToDisconnectList(int fd)
{
	_disconnect_list.push_back(fd);
}

void Server::deleteDisconnectedClients()
{
	for (std::size_t i = 0; i < _disconnect_list.size(); ++i)
		removeClient(_disconnect_list[i]);
	_disconnect_list.clear();
}

void Server::removeClient(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client* client = it->second;
	std::cout << "Client disconnected: " << client->hostname()
			  << " (fd " << fd << ")" << std::endl;

	const std::set<std::string> channels = client->joinedChannels();
	for (std::set<std::string>::const_iterator ch = channels.begin(); ch != channels.end(); ++ch)
	{
		Channel* channel = findChannel(*ch);
		if (channel)
		{
			channel->removeMember(client);
			deleteChannelIfEmpty(*ch);
		}
	}

	close(fd);
	delete client;
	_clients.erase(it);
	unregisterPfd(fd);
}

void Server::unregisterPfd(int fd)
{
	for (std::size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			_pfds.erase(_pfds.begin() + static_cast<std::ptrdiff_t>(i));
			return;
		}
	}
}

void Server::setPollout(int fd, bool enable)
{
	for (std::size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			if (enable)
				_pfds[i].events |= POLLOUT;
			else
				_pfds[i].events &= ~POLLOUT;
			return;
		}
	}
}
