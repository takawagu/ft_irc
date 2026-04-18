#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <sys/socket.h>

static ssize_t sendToClient(Client& client);
static void removeSentData(Client& client, std::size_t n);



void Server::handleClientWrite(int fd)
{
	Client* client = findClient(fd);
	if (!client)
		return;

	if (!client->hasDataToSend())
	{
		setPollout(fd, false);
		return;
	}

	ssize_t bytesSent = sendToClient(*client);
	if (!isSendSuccessful(bytesSent, fd))
		return;

	removeSentData(*client, static_cast<std::size_t>(bytesSent));

	if (!client->hasDataToSend())
		setPollout(fd, false);
}

bool Server::isSendSuccessful(ssize_t bytesSent, int fd)
{
	bool sendError   = (bytesSent < 0);
	bool nothingSent = (bytesSent == 0);
	bool unrecoverableError = (errno != EAGAIN && errno != EWOULDBLOCK);

	if (sendError)
	{
		if (unrecoverableError)
			addToDisconnectList(fd);
		return false;
	}
	if (nothingSent)
		return false;
	return true;
}

static ssize_t sendToClient(Client& client)
{
	return send(client.fd(), client.sendBuffer().data(), client.sendBuffer().size(), 0);
}

static void removeSentData(Client& client, std::size_t n)
{
	client.eraseSent(n);
}
