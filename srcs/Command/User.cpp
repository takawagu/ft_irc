#include "User.hpp"
#include "Server.hpp"
#include "Client.hpp"

void User::executeAction(Server& server, Client& client, int fd)
{
	if (params().size() < 4)
	{
		server.sendError(client, fd, "461", "USER :Not enough parameters");
		return;
	}
	if (client.isRegistered())
	{
		server.sendError(client, fd, "462", ":You may not reregister");
		return;
	}
	if (!isUsernameValid(params()[0]))
	{
		server.sendError(client, fd, "468", ":Your username is not valid");
		return;
	}
	client.setUsername(params()[0]);
	std::string realname = params()[3];
	if (!realname.empty() && realname[0] == ':')
		realname = realname.substr(1);
	client.setRealname(realname);
	if (client.tryRegister())
		server.sendWelcomeMessage(client, fd);
}

bool User::isUsernameValid(const std::string& user)
{
	if (user.empty())
		return false;
	for (size_t i = 0; i < user.size(); i++)
	{
		char c = user[i];
		if (c == '\0' || c == '\r' || c == '\n' || c == ' ' || c == '@')
			return false;
	}
	return true;
}
