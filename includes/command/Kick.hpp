#ifndef KICK_HPP
#define KICK_HPP

#include "ACommand.hpp"

class Kick : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
