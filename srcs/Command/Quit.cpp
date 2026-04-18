#include "Quit.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Quit::execute(Server& server, Client& client, int fd, const std::string& params)
{
	(void)server;
	(void)client;
	(void)fd;
	(void)params;
	// TODO: implement QUIT
}
