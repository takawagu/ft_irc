#include "Server.hpp"
#include "Client.hpp"

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

void Server::sendWelcomeMessage(Client& client, int fd)
{
	const std::string& nick = client.nickname();
	client.appendToSendBuffer(":ircserv 001 " + nick + " :Welcome to the Internet Relay Network " + client.prefix() + "\r\n");
	client.appendToSendBuffer(":ircserv 002 " + nick + " :Your host is ircserv, running version 1.0\r\n");
	client.appendToSendBuffer(":ircserv 003 " + nick + " :This server was created sometime\r\n");
	client.appendToSendBuffer(":ircserv 004 " + nick + " ircserv 1.0 o itkol\r\n");
	client.appendToSendBuffer(":ircserv 422 " + nick + " :MOTD File is missing\r\n");
	setPollout(fd, true);
}

void Server::sendError(Client& client, int fd, const std::string& code, const std::string& body)
{
	std::string nick = client.nickname();
	if (nick.empty())
		nick = "*";
	std::string reply = ":ircserv " + code + " " + nick + " " + body + "\r\n";
	client.appendToSendBuffer(reply);
	setPollout(fd, true);
}

bool Server::isSendSuccessful(ssize_t bytesSent, int fd)
{
	bool sendError   = (bytesSent < 0);
	bool nothingSent = (bytesSent == 0);

	if (sendError)
	{
		addToDisconnectList(fd);
		return false;
	}
	if (nothingSent)
		return false;
	return true;
}
