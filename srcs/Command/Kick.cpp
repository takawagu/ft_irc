#include "Kick.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Kick::executeAction(Server& server, Client& client, int fd)
{
	(void)server;
	(void)client;
	(void)fd;
	if (params().size() < 2)
	{
		server.sendError(client, fd, "461", "KICK :Not enough parameters");
		return;
	}
	Channel* channel = server.findChannel(params()[0]);
	if (channel == NULL)
	{
		server.sendError(client, fd, "403", params()[0] + " :No such channel");
		return;	
	}
	Client* targetClient = server.findClientByNick(params()[1]);
	if (targetClient == NULL)
	{
		server.sendError(client, fd, "401", params()[1] + " :No such nick/channel");
		return;	
	}
	if (!channel->hasMember(&client))
	{
		server.sendError(client, fd, "442", params()[0] + " :You're not on that channel");
		return;	
	}
	if (!channel->isOperator(&client))
	{
		server.sendError(client, fd, "482", params()[0] + " :You're not channel operator");
		return;	
	}
	if (!channel->hasMember(targetClient))
	{		
		server.sendError(client, fd, "441", params()[1] + " " + params()[0] + " :They aren't on that channel");
		return;
	}
	channel->removeMember(targetClient);
	std::string kickMsg;
	if (params().size() == 2)
		kickMsg = ":" + client.prefix() + " KICK " + channel->name() + " " + targetClient->nickname();
	else
		kickMsg = ":" + client.prefix() + " KICK " + channel->name() + " " + targetClient->nickname() + " :" + params()[2];
	server.broadcastToChannel(channel, kickMsg);
}