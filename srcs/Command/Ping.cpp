#include "Ping.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Ping::executeAction(Server& server, Client& client, int fd)
{
	std::string token;
	if (params().empty())
		token = "ircserv";
	else
	{
		token = params()[0];
		if (!token.empty() && token[0] == ':')
			token = token.substr(1);
	}
	client.appendToSendBuffer(":ircserv PONG ircserv :" + token + "\r\n");
	server.setPollout(fd, true);
}
