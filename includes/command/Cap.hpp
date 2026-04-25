#ifndef CAP_HPP
#define CAP_HPP

#include "ACommand.hpp"

class Cap : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
