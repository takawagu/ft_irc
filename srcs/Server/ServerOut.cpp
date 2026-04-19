#include "Server.hpp"
#include "Client.hpp"

#include <cerrno>
#include <sys/socket.h>

static int FLAGS_NONE = 0;

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

	ssize_t bytesSent = send(client->fd(), client->sendBuffer().data(), client->sendBuffer().size(), FLAGS_NONE);
	if (!isSendSuccessful(bytesSent, fd))
		return;

	client->removeSentData(static_cast<std::size_t>(bytesSent));

	if (!client->hasDataToSend())
		setPollout(fd, false);
}

bool Server::isSendSuccessful(ssize_t bytesSent, int fd)
{
	bool sendError   = (bytesSent < 0);
	bool nothingSent = (bytesSent == 0);

	if (sendError)
	{
		bool unrecoverableError = (errno != EAGAIN && errno != EWOULDBLOCK);
		if (unrecoverableError)
			addToDisconnectList(fd);
		return false;
	}
	if (nothingSent)
		return false;
	return true;
}
