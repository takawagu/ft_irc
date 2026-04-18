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
	struct sockaddr_in client_addr;

	int client_fd = acceptConnection(client_addr);

	if (client_fd < 0)
		return;

	registerClient(client_fd, client_addr);
}

int Server::acceptConnection(struct sockaddr_in& addr)
{
	socklen_t len = sizeof(addr);
	int client_fd = accept(_listen_fd, reinterpret_cast<struct sockaddr*>(&addr), &len);

	if (!isAcceptSuccessful(client_fd))
		return -1;
	if (!setNonBlocking(client_fd))
	{
		close(client_fd);
		return -1;
	}
	return client_fd;
}

bool Server::isAcceptSuccessful(int client_fd)
{
	if (client_fd >= 0)
		return true;
	bool unrecoverableError = (errno != EAGAIN && errno != EWOULDBLOCK);
	if (unrecoverableError)
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
	return false;
}

bool Server::setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		return false;
	}
	return true;
}

void Server::registerClient(int client_fd, const struct sockaddr_in& addr)
{
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));

	_clients[client_fd] = new Client(client_fd, ip);
	registerPfd(client_fd, POLLIN);

	std::cout << "Client connected: " << ip << " (fd " << client_fd << ")" << std::endl;
}
