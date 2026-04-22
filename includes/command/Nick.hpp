#ifndef NICK_HPP
#define NICK_HPP

#include "ACommand.hpp"

#define MAX_NICKNAME_LENGTH 9

class Nick : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);
	private:
		bool isNicknameValid(const std::string& nick);
};

#endif
