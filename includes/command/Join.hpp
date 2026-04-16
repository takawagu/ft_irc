#ifndef JOIN_HPP
#define JOIN_HPP

#include "ACommand.hpp"

class Join : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
