#ifndef JOIN_HPP
#define JOIN_HPP

#include "ACommand.hpp"

class Join : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
