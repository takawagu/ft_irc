#ifndef INVITE_HPP
#define INVITE_HPP

#include "ACommand.hpp"

class Invite : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
