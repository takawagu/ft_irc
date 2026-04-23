#include "Pass.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Pass::executeAction(Server& server, Client& client, int fd)
{
	if (client.isRegistered())
	{
		server.sendError(client, fd, "462", " :You may not reregister");
		return;
	}
	if (params().size() <= 1)
	{
		server.sendError(client, fd, "461", "PASS :Not enough parameters");
		return;
	}
	if (!server.checkPassword(params()[1]))
	{
		server.sendError(client, fd, "464", " :Password incorrect");
		return;
	}
	client.setPassAccepted();
}
