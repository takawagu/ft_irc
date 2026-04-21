#ifndef MODE_HPP
#define MODE_HPP

#include "ACommand.hpp"

class Mode : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
