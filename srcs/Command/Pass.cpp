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
	if (params().empty())
	{
		server.sendError(client, fd, "461", "PASS :Not enough parameters");
		return;
	}
	std::string pass = params()[0];
	if (!pass.empty() && pass[0] == ':')
		pass = pass.substr(1);
	if (!server.checkPassword(pass))
	{
		server.sendError(client, fd, "464", " :Password incorrect");
		server.addToDisconnectList(fd);
		return;
	}
	client.setPassAccepted();
}
