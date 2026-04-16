#ifndef PONG_HPP
#define PONG_HPP

#include "ACommand.hpp"

class Pong : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
