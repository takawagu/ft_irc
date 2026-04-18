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
	ssize_t bytesReceived = recvFromClient(fd, buf, sizeof(buf));
	if (!isRecvSuccessful(bytesReceived, fd))
		return;

	appendToRecvBuffer(*client, buf, static_cast<std::size_t>(bytesReceived));
	if (!isRecvBufferAvailable(*client, fd))
		return;

	processClientRequests(*client, fd);
}

bool Server::isRecvSuccessful(ssize_t bytesReceived, int fd)
{
	bool recvError        = (bytesReceived < 0);
	bool connectionClosed = (bytesReceived == 0);
	bool unrecoverableError = (errno != EAGAIN && errno != EWOULDBLOCK);

	if (recvError)
	{
		if (unrecoverableError)
			addToDisconnectList(fd);
		return false;
	}
	if (connectionClosed)
	{
		addToDisconnectList(fd);
		return false;
	}
	return true;
}

bool Server::isRecvBufferAvailable(Client& client, int fd)
{
	if (client.isRecvBufferFull())
	{
		addToDisconnectList(fd);
		return false;
	}
	return true;
}

void Server::processClientRequests(Client& client, int fd)
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
