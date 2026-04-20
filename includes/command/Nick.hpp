#ifndef NICK_HPP
#define NICK_HPP

#include "ACommand.hpp"

class Nick : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
