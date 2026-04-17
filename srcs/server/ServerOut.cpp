#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <sys/socket.h>

static ssize_t sendToClient(Client& client);
static void flushSentData(Client& client, std::size_t n);

void Server::handleClientWrite(int fd)
{
	Client* client = findClient(fd);
	if (!client)
		return;

	if (!client->hasPendingSend())
	{
		setPollout(fd, false);
		return;
	}

	ssize_t n = sendToClient(*client);
	if (!handleSendResult(n, fd))
		return;

	flushSentData(*client, static_cast<std::size_t>(n));
	if (!client->hasPendingSend())
		setPollout(fd, false);
}

bool Server::handleSendResult(ssize_t n, int fd)
{
	if (n < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return false;
		addToDisconnectList(fd);
		return false;
	}
	if (n == 0)
		return false;
	return true;
}

static ssize_t sendToClient(Client& client)
{
	return send(client.fd(), client.sendBuffer().data(), client.sendBuffer().size(), 0);
}

static void flushSentData(Client& client, std::size_t n)
{
	client.eraseSent(n);
}
