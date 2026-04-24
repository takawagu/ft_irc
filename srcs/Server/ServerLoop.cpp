#include "Server.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

static bool isReadable(short revents);
static bool isWritable(short revents);
static bool hasError(short revents);
static bool isDisconnected(short revents);


void Server::loop()
{
	while (!_stop)
	{
		waitForEvents();
		handlePollEvents();
		deleteDisconnectedClients();
	}
}

void Server::waitForEvents()
{
	int ready = -1;

	while (ready < 0)
	{
		ready = poll(&_pfds[0], _pfds.size(), -1);
		if (ready < 0 && errno != EINTR)
			throw std::runtime_error("poll: " + std::string(std::strerror(errno)));
	}
}

void Server::handlePollEvents()
{
	for (std::size_t i = 0; i < _pfds.size(); ++i)
	{
		short revents = _pfds[i].revents;

		if (revents)
		{
			int fd = _pfds[i].fd;

			if (isNewConnection(fd, revents))
				acceptNewClient();
			else if (hasError(revents))
				addToDisconnectList(fd);
			else
				handleClientEvents(fd, revents);
		}
	}
}

void Server::handleClientEvents(int fd, short revents)
{
	if (isReadable(revents))
		handleClientRead(fd);
	if (isWritable(revents))
		handleClientWrite(fd);
	if (isDisconnected(revents))
		addToDisconnectList(fd);
}

bool Server::isNewConnection(int fd, short revents) const
{
	return fd == _listen_fd && isReadable(revents);
}

static bool isReadable(short revents)
{
	return revents & POLLIN;
}

static bool isWritable(short revents)
{
	return revents & POLLOUT;
}

static bool hasError(short revents)
{
	return revents & (POLLERR | POLLNVAL);
}

static bool isDisconnected(short revents)
{
	return revents & POLLHUP;
}
