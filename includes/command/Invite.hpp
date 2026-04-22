#ifndef INVITE_HPP
#define INVITE_HPP

#include "ACommand.hpp"

class Invite : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
