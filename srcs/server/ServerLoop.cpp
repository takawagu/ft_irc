#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <sys/socket.h>

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
		flushPendingDisconnects();
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
		short re = _pfds[i].revents;
		if (re == 0)
			continue;

		int fd = _pfds[i].fd;

		if (fd == _listen_fd)
		{
			if (re & POLLIN)
				acceptNewClient();
			continue;
		}

		if (re & (POLLERR | POLLNVAL))
		{
			disconnectClient(fd);
			continue;
		}
		if (re & POLLIN)
			handleClientRead(fd);
		if (re & POLLOUT)
			handleClientWrite(fd);
		if (re & POLLHUP)
			disconnectClient(fd);
	}
}

void Server::handleClientRead(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client& client = *it->second;
	char buf[4096];
	ssize_t n = recv(fd, buf, sizeof(buf), 0);

	if (n < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		disconnectClient(fd);
		return;
	}
	if (n == 0)
	{
		disconnectClient(fd);
		return;
	}

	client.appendRecv(buf, static_cast<std::size_t>(n));

	if (client.isRecvBufferFull())
	{
		disconnectClient(fd);
		return;
	}

	std::string line;
	while (client.extractLine(line))
		parseLine(client, fd, line);
}

void Server::parseLine(Client& client, int fd, const std::string& line)
{
	std::cout << "fd " << fd << " >> " << line << std::endl;

	std::string::size_type pos = 0;
	while (pos < line.size() && line[pos] == ' ')
		++pos;
	if (pos == line.size())
		return;

	std::string::size_type cmd_end = line.find(' ', pos);
	if (cmd_end == std::string::npos)
		cmd_end = line.size();

	std::string command = line.substr(pos, cmd_end - pos);

	std::string::size_type arg_pos = cmd_end;
	while (arg_pos < line.size() && line[arg_pos] == ' ')
		++arg_pos;

	std::string params = (arg_pos < line.size()) ? line.substr(arg_pos) : std::string();

	handleCommand(client, fd, command, params);
}

void Server::handleClientWrite(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client& client = *it->second;
	if (!client.hasPendingSend())
	{
		setPollout(fd, false);
		return;
	}

	ssize_t n = send(fd, client.sendBuffer().data(), client.sendBuffer().size(), 0);
	if (n < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		disconnectClient(fd);
		return;
	}
	if (n == 0)
		return;

	client.eraseSent(static_cast<std::size_t>(n));
	if (!client.hasPendingSend())
		setPollout(fd, false);
}
