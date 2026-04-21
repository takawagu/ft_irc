#ifndef PONG_HPP
#define PONG_HPP

#include "ACommand.hpp"

class Pong : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
