#include "Part.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Part::executeAction(Server& server, Client& client, int fd)
{
	if (params().empty() || params()[0][0] == ':')
	{
		server.sendError(client, fd, "461", "PART :Not enough parameters");
		return;
	}
	Channel* channel = server.findChannel(params()[0]);
	if (channel == NULL)
	{
		server.sendError(client, fd, "403", params()[0] + " :No such channel");
		return;	
	}
	std::string partMsg;
	if (params().size() == 2)
		partMsg = ":" + client.prefix() + " PART " + channel->name();
	else
		partMsg = ":" + client.prefix() + " PART " + channel->name() + " :" + params()[2];
	server.broadcastToChannel(channel, partMsg);
	channel->removeMember(&client);
	server.deleteChannelIfEmpty(channel->name());
}
