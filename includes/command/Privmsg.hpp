#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#include "ACommand.hpp"

class Privmsg : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
	private:
		bool paramsErrorCheck(Server& server, Client& client, int fd);
};

#endif
