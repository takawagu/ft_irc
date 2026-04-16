#ifndef USER_HPP
#define USER_HPP

#include "ACommand.hpp"

class User : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
