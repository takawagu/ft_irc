#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <string>

class Server;
class Client;

class ACommand
{
	public:
		virtual ~ACommand() {}
		virtual void execute(Server& server, Client& client, int fd, const std::string& params) = 0;
};

#endif
