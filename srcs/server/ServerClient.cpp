#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

void Server::acceptNewClient()
{
	struct sockaddr_in cli_addr;
	int cfd = acceptConnection(cli_addr);
	if (cfd < 0)
		return;
	registerClient(cfd, cli_addr);
}

int Server::acceptConnection(struct sockaddr_in& addr)
{
	socklen_t len = sizeof(addr);
	int cfd = accept(_listen_fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
	
	if (cfd < 0)
	{
		int saved_errno = errno;
		if (saved_errno != EAGAIN && saved_errno != EWOULDBLOCK)
			std::cerr << "accept: " << std::strerror(saved_errno) << std::endl;
		return -1;
	}

	if (fcntl(cfd, F_SETFL, O_NONBLOCK) < 0)
	{
		int saved_errno = errno;
		std::cerr << "fcntl: " << std::strerror(saved_errno) << std::endl;
		close(cfd);
		return -1;
	}
	
	return cfd;
}

void Server::registerClient(int cfd, const struct sockaddr_in& addr)
{
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

	_clients[cfd] = new Client(cfd, ip);
	registerPfd(cfd, POLLIN);

	std::cout << "Client connected: " << ip << " (fd " << cfd << ")" << std::endl;
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

	std::cout << "Client disconnected: " << it->second->hostname()
			  << " (fd " << fd << ")" << std::endl;

	close(fd);
	delete it->second;
	_clients.erase(it);

	for (std::size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			_pfds.erase(_pfds.begin() + static_cast<std::ptrdiff_t>(i));
			break;
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
