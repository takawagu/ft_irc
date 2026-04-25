#include "User.hpp"
#include "Server.hpp"
#include "Client.hpp"

static void sendWelcomeMessage(Server& server, Client& client, int fd);

void User::executeAction(Server& server, Client& client, int fd)
{
	if (params().size() < 4)
	{
		server.sendError(client, fd, "461", "USER :Not enough parameters");
		return;
	}
	if (client.tryRegister())
	{
		server.sendError(client, fd, "462", ":You may not reregister");
		return;
	}
	client.setUsername(params()[0]);
	client.setRealname(params()[3]);
	if (client.tryRegister())
		sendWelcomeMessage(server, client, fd);
}

static void sendWelcomeMessage(Server& server, Client& client, int fd)
{
	std::string msg = ":ircserv 001 " + client.nickname() + " :Welcome to the Internet Relay Network " + client.prefix() + "\r\n";
	client.appendToSendBuffer(msg);
	server.setPollout(fd, true);
}
