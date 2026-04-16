#ifndef NICK_HPP
#define NICK_HPP

#include "ACommand.hpp"

class Nick : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
