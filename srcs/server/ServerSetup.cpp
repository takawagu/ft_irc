#include "Server.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>

static int checkSyscall(int ret, const char* name);
static void registerSignalHandler(void (*handler)(int));
static void ignoreSigpipe();

void Server::setup()
{
	_listen_fd = checkSyscall(socket(AF_INET, SOCK_STREAM, 0), "socket");

	int opt = 1;
	checkSyscall(setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), "setsockopt");
	checkSyscall(fcntl(_listen_fd, F_SETFL, O_NONBLOCK), "fcntl");

	struct sockaddr_in addr = createListenAddr();
	checkSyscall(bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)), "bind");
	checkSyscall(listen(_listen_fd, SOMAXCONN), "listen");

	registerPfd(_listen_fd, POLLIN);
	setupSignals();

	std::cout << "Server listening on port " << _config.port() << std::endl;
}

static int checkSyscall(int ret, const char* name)
{
	if (ret < 0)
	{
		int saved_errno = errno;
		throw std::runtime_error(std::string(name) + ": " + std::strerror(saved_errno));
	}
	return ret;
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

void Server::setupSignals()
{
	registerSignalHandler(signalHandler);
	ignoreSigpipe();
}

static void registerSignalHandler(void (*handler)(int))
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

static void ignoreSigpipe()
{
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGPIPE, &sa, NULL);
}

void Server::registerPfd(int fd, short events)
{
	pollfd pfd;
	pfd.fd      = fd;
	pfd.events  = events;
	pfd.revents = 0;
	_pfds.push_back(pfd);
}
