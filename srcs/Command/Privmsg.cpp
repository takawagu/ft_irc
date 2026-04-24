#include "Privmsg.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Nick.hpp"
#include "Channel.hpp"

//    Parameters: <receiver>{,<receiver>} <text to be sent>
void Privmsg::executeAction(Server& server, Client& client, int fd)
{
	if (params().empty())
	{
		server.sendError(client, fd, "461", "PRIVMSG :Not enough parameters");
		return;
	}
	if (params()[0][0] == ':')
	{
		server.sendError(client, fd, "411", "nick :No recipient given (PRIVMSG)");
		return;
	}
	if(params().size() < 2)
	{
		server.sendError(client, fd, "412", "nick :No text to send");
		return;
	}
	if (params()[0][0] == '#')
	{
		std::string channel_name = params()[0].substr(1);
		Channel* channel = server.findChannel(channel_name);
		if (channel == NULL)
		{
			server.sendError(client, fd, "403", params()[0] + " :No such channel");
			return;
		}
		if (!channel->hasMember(&client))
		{
			server.sendError(client, fd, "404", params()[0] + " :Cannot send to channel");
			return;
		}
		std::string msg = ":" + client.nickname() + " PRIVMSG " + params()[0] + " :" + params()[1] + "\r\n";
		server.broadcastToChannel(channel, msg, &client);
		return;
	}
	else
	{
		Client* target = server.findClientByNick(params()[0]);
		if (target == NULL)
		{
			server.sendError(client, fd, "401", params()[0] + " :No such nick/channel");
			return;
		}
		std::string msg = ":" + client.nickname() + " PRIVMSG " + params()[0] + " :" + params()[1] + "\r\n";
		target->appendToSendBuffer(msg);
		server.setPollout(target->fd(), true);
	}
}
