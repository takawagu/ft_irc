#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ACommand.hpp"

class Topic : public ACommand
{
	public:
		void execute(Server& server, Client& client, int fd, const std::string& params);
};

#endif
