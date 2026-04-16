#ifndef PING_HPP
#define PING_HPP

#include "ACommand.hpp"

class Ping : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
