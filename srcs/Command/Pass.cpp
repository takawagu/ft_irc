#include "Pass.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Pass::execute(Server& server, Client& client, int fd, const std::string& params)
{
	(void)server;
	(void)client;
	(void)fd;
	if (params.empty())
	{
		return;
	}
	// TODO: implement PASS
}
