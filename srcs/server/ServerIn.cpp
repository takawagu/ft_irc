#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <sys/socket.h>

static const std::size_t RECV_BUF_SIZE = 4096;

static ssize_t recvFromClient(int fd, char* buf, std::size_t bufSize);
static void appendToRecvBuffer(Client& client, const char* buf, std::size_t n);

void Server::handleClientRead(int fd)
{
	Client* client = findClient(fd);
	if (!client)
		return;

	char buf[RECV_BUF_SIZE];
	ssize_t n = recvFromClient(fd, buf, sizeof(buf));
	if (!handleRecvResult(n, fd))
		return;

	appendToRecvBuffer(*client, buf, static_cast<std::size_t>(n));
	if (checkRecvBufferOverflow(*client, fd))
		return;

	processClientCommands(*client, fd);
}

bool Server::handleRecvResult(ssize_t n, int fd)
{
	if (n < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;
		addToDisconnectList(fd);
		return false;
	}
	if (n == 0)
	{
		addToDisconnectList(fd);
		return false;
	}
	return true;
}

bool Server::checkRecvBufferOverflow(Client& client, int fd)
{
	if (client.isRecvBufferFull())
	{
		addToDisconnectList(fd);
		return true;
	}
	return false;
}

void Server::processClientCommands(Client& client, int fd)
{
	std::string line;
	while (client.extractLine(line))
	{
		std::string command;
		std::string params;
		if (parseRequest(line, command, params))
			handleCommand(client, fd, command, params);
	}
}

static ssize_t recvFromClient(int fd, char* buf, std::size_t bufSize)
{
	return recv(fd, buf, bufSize, 0);
}

static void appendToRecvBuffer(Client& client, const char* buf, std::size_t n)
{
	client.appendRecv(buf, n);
}
