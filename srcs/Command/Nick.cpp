#include "Nick.hpp"
#include "Server.hpp"
#include "Client.hpp"

void Nick::executeAction(Server& server, Client& client, int fd)
{
	if (!client.isPassAccepted())
	{
		server.sendError(client, fd, "464", ":Password required");
		return;
	}
	if (params().empty())
	{
		server.sendError(client, fd, "431", ":No nickname given");
		return;
	}
	if (!isNicknameValid(params()[0]))
	{
		server.sendError(client, fd, "432", params()[0] + " :Erroneus nickname");
		return;
	}
	if (server.isNicknameTaken(params()[0]))
	{
		server.sendError(client, fd, "433", params()[0] + " :Nickname is already in use");
		return;
	}
	std::string old_nick = client.nickname();
	client.setNickname(params()[0]);
	std::string nick_msg = ":" + old_nick + " NICK " + client.nickname() + "\r\n";
	const std::set<std::string>& channels = client.joinedChannels();
	if (channels.empty())
	{
		client.appendToSendBuffer(nick_msg);
		server.setPollout(fd, true);
	}
	else
	{
		for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it)
		{
			Channel* channel = server.findChannel(*it);
			if (channel)
				server.broadcastToChannel(channel, nick_msg, NULL);
		}
	}
	if (client.tryRegister())
		server.sendWelcomeMessage(client, fd);
}

static bool isNicknameSymbol(char c);

bool Nick::isNicknameValid(const std::string& nick)
{
	if (nick.empty() || nick.length() > MAX_NICKNAME_LENGTH)
		return false;
	if (!isalpha(nick[0]))
		return false;
	for (size_t i = 1; i < nick.length(); i++)
	{
		if (!isalnum(nick[i]) && !isNicknameSymbol(nick[i]))
			return false;
	}
	return true;
}

static bool isNicknameSymbol(char c)
{
	return c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}' ;
}

