#include "Pass.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Pass::executeAction(Server& server, Client& client, int fd)
{
	(void)fd;
	(void)server;

	if (params().size() <= 1)
	{
		std::string error_msg = ":" + client.hostname() + " 461 " + client.nickname() + " PASS :Not enough parameters\r\n";
		client.appendToSendBuffer(error_msg);
		return;
	}
	std::cout << "Received PASS command with params: " << params()[1] << std::endl;
	// if (params == server.config().password())
	// {
	// 	client.setRegistered(true);
	// }
	// else
	// {
	// 	std::string error_msg = ":" + client.hostname() + " 464 " + client.nickname() + " :Password incorrect\r\n";
	// 	client.queueSend(error_msg);
	// }
	return;
}
