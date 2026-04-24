#include "Privmsg.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Nick.hpp"
#include "Channel.hpp"

static void sendForChannel(Server& server, Client& client, int fd, const std::string& channel_name, std::string message);
static void sendForTarget(Server& server, Client& client, int fd, const std::string& target, std::string message);

//    Parameters: <receiver>{,<receiver>} <text to be sent>
void Privmsg::executeAction(Server& server, Client& client, int fd)
{
	if (!paramsErrorCheck(server, client, fd))
		return;

	std::vector<std::string> receivers;
	receivers = splitByComma(params()[0]);

	for (std::vector<std::string>::iterator it = receivers.begin(); it != receivers.end(); ++it)
	{
		if (it->at(0) == '#')
		{
			std::string channel_name = *it;
			sendForChannel(server, client, fd, channel_name, params()[1]);
		}
		else
		{
			sendForTarget(server, client, fd, *it, params()[1]);
		}	
	}
	return;
}

bool Privmsg::paramsErrorCheck(Server& server, Client& client, int fd)
{
	if (params().empty())
	{
		server.sendError(client, fd, "461", "PRIVMSG :Not enough parameters");
		return false;
	}
	if (params()[0][0] == ':')
	{
		server.sendError(client, fd, "411", "nick :No recipient given (PRIVMSG)");
		return false;
	}
	if(params().size() < 2)
	{
		server.sendError(client, fd, "412", "nick :No text to send");
		return false;
	}
	return true;
}

void sendForChannel(Server& server, Client& client, int fd, const std::string& channel_name, std::string message)
{
	Channel* channel = server.findChannel(channel_name);
	if (channel == NULL)
	{
		server.sendError(client, fd, "403", channel_name + " :No such channel");
		return;
	}
	if (!channel->hasMember(&client))
	{
		server.sendError(client, fd, "404", channel_name + " :Cannot send to channel");
		return;
	}
	std::string msg = ":" + client.nickname() + " PRIVMSG " + channel_name + " :" + message + "\r\n";
	server.broadcastToChannel(channel, msg, &client);
}

void sendForTarget(Server& server, Client& client, int fd, const std::string& target, std::string message)
{
	Client* target_client = server.findClientByNick(target);
	if (target_client == NULL)
	{
		server.sendError(client, fd, "401", target + " :No such nick/channel");
		return;
	}
	std::string msg = ":" + client.nickname() + " PRIVMSG " + target + " :" + message + "\r\n";
	target_client->appendToSendBuffer(msg);
	server.setPollout(target_client->fd(), true);
}
