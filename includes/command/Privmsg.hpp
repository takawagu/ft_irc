#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#include "ACommand.hpp"

class Privmsg : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
