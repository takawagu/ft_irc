#include "Pass.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Pass::executeAction(Server& server, Client& client, int fd)
{
	(void)fd;

	if (params().size() <= 1)
	{
		std::string error_msg = ":" + client.hostname() + " 461 " + client.nickname() + " PASS :Not enough parameters\r\n";
		client.appendToSendBuffer(error_msg);
		return;
	}
	if (server.checkPassword(params()[1]))
	{
		client.setRegistered(true);
		std::cout << "success " << std::endl;
	}
	else
	{
		std::string error_msg = ":" + client.hostname() + " 464 " + client.nickname() + " :Password incorrect\r\n";
		client.appendToSendBuffer(error_msg);
	}
	return;
}
