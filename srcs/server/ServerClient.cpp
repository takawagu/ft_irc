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
	socklen_t cli_len = sizeof(cli_addr);
	int cfd = accept(_listen_fd, reinterpret_cast<struct sockaddr*>(&cli_addr), &cli_len);
	if (cfd < 0)
	{
		int saved_errno = errno;
		if (saved_errno == EAGAIN || saved_errno == EWOULDBLOCK)
			return;
		std::cerr << "accept: " << std::strerror(saved_errno) << std::endl;
		return;
	}

	if (fcntl(cfd, F_SETFL, O_NONBLOCK) < 0)
	{
		int saved_errno = errno;
		std::cerr << "fcntl: " << std::strerror(saved_errno) << std::endl;
		close(cfd);
		return;
	}

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cli_addr.sin_addr, ip, sizeof(ip));

	Client* client = new Client(cfd, ip);
	_clients[cfd] = client;

	pollfd pfd;
	pfd.fd      = cfd;
	pfd.events  = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);

	std::cout << "Client connected: " << ip << " (fd " << cfd << ")" << std::endl;
}

void Server::disconnectClient(int fd)
{
	_pending_disconnect.push_back(fd);
}

void Server::flushPendingDisconnects()
{
	for (std::size_t i = 0; i < _pending_disconnect.size(); ++i)
		removeClient(_pending_disconnect[i]);
	_pending_disconnect.clear();
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
