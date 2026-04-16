#include "Server.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

void Server::setup()
{
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
	{
		int saved_errno = errno;
		throw std::runtime_error("socket: " + std::string(std::strerror(saved_errno)));
	}

	int opt = 1;
	if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		int saved_errno = errno;
		throw std::runtime_error("setsockopt: " + std::string(std::strerror(saved_errno)));
	}

	if (fcntl(_listen_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		int saved_errno = errno;
		throw std::runtime_error("fcntl: " + std::string(std::strerror(saved_errno)));
	}

	struct sockaddr_in addr = createListenAddr();

	if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
	{
		int saved_errno = errno;
		throw std::runtime_error("bind: " + std::string(std::strerror(saved_errno)));
	}

	if (listen(_listen_fd, SOMAXCONN) < 0)
	{
		int saved_errno = errno;
		throw std::runtime_error("listen: " + std::string(std::strerror(saved_errno)));
	}

	registerListenPfd();

	std::cout << "Server listening on port " << _config.port() << std::endl;
}

struct sockaddr_in Server::createListenAddr() const
{
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port        = htons(static_cast<uint16_t>(_config.port()));
	return addr;
}

void Server::registerListenPfd()
{
	pollfd listen_pfd;
	listen_pfd.fd      = _listen_fd;
	listen_pfd.events  = POLLIN;
	listen_pfd.revents = 0;
	_pfds.push_back(listen_pfd);
}
