#include "Cap.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Cap::executeAction(Server& server, Client& client, int fd)
{
	if (params().empty())
		return;

	const std::string& subcommand = params()[0];
	if (subcommand == "LS")
	{
		client.appendToSendBuffer("CAP * LS :\r\n");
		server.setPollout(fd, true);
	}
}
