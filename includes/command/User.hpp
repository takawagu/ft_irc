#ifndef USER_HPP
#define USER_HPP

#include "ACommand.hpp"

class User : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
