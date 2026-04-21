#ifndef QUIT_HPP
#define QUIT_HPP

#include "ACommand.hpp"

class Quit : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
