#include "Server.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>

void Server::loop()
{
	setupSignals();

	while (!_stop)
	{
		int ready = poll(&_pfds[0], _pfds.size(), -1);
		if (ready < 0)
		{
			int saved_errno = errno;
			if (saved_errno == EINTR)
				continue;
			throw std::runtime_error("poll: " + std::string(std::strerror(saved_errno)));
		}

		handlePollEvents();
		deleteDisconnectedClients();
	}
}

void Server::setupSignals()
{
	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);

	struct sigaction sa_ign;
	sa_ign.sa_handler = SIG_IGN;
	sigemptyset(&sa_ign.sa_mask);
	sa_ign.sa_flags = 0;
	sigaction(SIGPIPE, &sa_ign, NULL);
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

bool Server::isReadable(short revents)
{
	return revents & POLLIN;
}

bool Server::isWritable(short revents)
{
	return revents & POLLOUT;
}

bool Server::hasError(short revents)
{
	return revents & (POLLERR | POLLNVAL);
}

bool Server::isDisconnected(short revents)
{
	return revents & POLLHUP;
}
