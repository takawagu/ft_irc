#ifndef TOPIC_HPP
#define TOPIC_HPP

#include "ACommand.hpp"

class Topic : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
};

#endif
