#ifndef PART_HPP
#define PART_HPP

#include "ACommand.hpp"

class Part : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
