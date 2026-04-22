#ifndef PING_HPP
#define PING_HPP

#include "ACommand.hpp"

class Ping : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
