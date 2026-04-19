#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <sys/socket.h>

static const std::size_t RECV_BUF_SIZE = 4096;
static int FLAGS_NONE = 0;


void Server::handleClientRead(int fd)
{
	Client* client = findClient(fd);
	if (!client)
		return;

	char buf[RECV_BUF_SIZE];
	ssize_t bytesReceived = recv(fd, buf, sizeof(buf), FLAGS_NONE);
	if (!isRecvSuccessful(bytesReceived, fd))
		return;

	client->appendToRecvBuffer(buf, static_cast<std::size_t>(bytesReceived));
	if (!isRecvBufferAvailable(*client, fd))
		return;

	processClientRequests(*client, fd);
}

bool Server::isRecvSuccessful(ssize_t bytesReceived, int fd)
{
	bool recvError        = (bytesReceived < 0);
	bool connectionClosed = (bytesReceived == 0);

	if (recvError)
	{
		bool unrecoverableError = (errno != EAGAIN && errno != EWOULDBLOCK);
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
