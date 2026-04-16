#include "Privmsg.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Privmsg::execute(Server& server, Client& client, int fd, const std::string& params)
{
	(void)server;
	(void)client;
	(void)fd;
	(void)params;
	// TODO: implement PRIVMSG
}
