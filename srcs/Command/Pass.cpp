#include "Pass.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Pass::executeAction(Server& server, Client& client, int fd)
{
	(void)fd;

	if (params().size() <= 1)
	{
		server.sendError(client, fd, "461", "PASS :Not enough parameters");
		return;
	}
	if (server.checkPassword(params()[1]))
	{
		client.setRegistered(true);
		std::cout << "success " << std::endl;
	}
	else
	{
		server.sendError(client, fd, "464", " :Password incorrect");
	}
	return;
}
