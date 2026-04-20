#include "Pass.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include <iostream>

void Pass::executeAction(Server& server, Client& client, int fd)
{
	(void)fd;

	if (params().size() <= 1)
	{
		std::string error_msg = "461 ERR_NEEDMOREPARAMS PASS :Not enough parameters\r\n";
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
		std::string error_msg = "464 ERR_PASSWDMISMATCH :Password incorrect\r\n";
		client.appendToSendBuffer(error_msg);
	}
	return;
}
