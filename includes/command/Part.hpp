#ifndef PART_HPP
#define PART_HPP

#include "ACommand.hpp"

class Part : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
