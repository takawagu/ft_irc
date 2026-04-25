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
	if (client.isRegistered())
	{
		server.sendError(client, fd, "462", ":You may not reregister");
		return;
	}
	if (!isUsernameValid(params()[0]))
	{
		server.sendError(client, fd, "468", ":Your username is not valid");
		return;
	}
	client.setUsername(params()[0]);
	client.setRealname(params()[3]);
	if (client.tryRegister())
		sendWelcomeMessage(server, client, fd);
}

bool User::isUsernameValid(const std::string& user)
{
	if (user.empty())
		return false;
	for (size_t i = 0; i < user.size(); i++)
	{
		char c = user[i];
		if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == '@')
			return false;
	}
	return true;
}

static void sendWelcomeMessage(Server& server, Client& client, int fd)
{
	const std::string& nick = client.nickname();
	client.appendToSendBuffer(":ircserv 001 " + nick + " :Welcome to the Internet Relay Network " + client.prefix() + "\r\n");
	client.appendToSendBuffer(":ircserv 002 " + nick + " :Your host is ircserv, running version 1.0\r\n");
	client.appendToSendBuffer(":ircserv 003 " + nick + " :This server was created sometime\r\n");
	client.appendToSendBuffer(":ircserv 004 " + nick + " ircserv 1.0 o itkol\r\n");
	client.appendToSendBuffer(":ircserv 422 " + nick + " :MOTD File is missing\r\n");
	server.setPollout(fd, true);
}
