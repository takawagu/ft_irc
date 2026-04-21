#ifndef PASS_HPP
#define PASS_HPP

#include "ACommand.hpp"

class Pass : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
