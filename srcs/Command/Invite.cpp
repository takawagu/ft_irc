#include "Invite.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Invite::executeAction(Server& server, Client& client, int fd)
{
	if (params().size() < 2)
	{
		server.sendError(client, fd, "461", "INVITE :Not enough parameters");
		return;
	}
	Client* targetClient = server.findClientByNick(params()[1]);
	if (targetClient == NULL)
	{
		server.sendError(client, fd, "401", params()[1] + " :No such nick/channel");
		return;
	}
	Channel* channel = server.findChannel(params()[0]);
	if (channel == NULL)
	{
		server.sendError(client, fd, "403", params()[0] + " :No such channel");
		return;
	}
	if (!channel->hasMember(&client))
	{
		server.sendError(client, fd, "442", params()[0] + " :You're not on that channel");
		return;
	}
	if (channel->isInviteOnly() && !channel->isOperator(&client))
	{
		server.sendError(client, fd, "482", params()[0] + " :You're not channel operator");
		return;
	}
	if (channel->hasMember(targetClient))
	{
		server.sendError(client, fd, "443", params()[1] + " " + params()[0] + " :User is already on that channel");
		return;
	}
	if (channel->isInvited(targetClient->nickname()))
		return;
	channel->addInvite(targetClient->nickname());
	std::string inviteMsg = ":" + client.prefix() + " INVITE " + targetClient->nickname() + " :" + channel->name() + "\r\n";
	targetClient->appendToSendBuffer(inviteMsg);
	server.setPollout(targetClient->fd(), true);
	std::string successMsg = ":ircserv 341 " + client.nickname() + " " + targetClient->nickname() + " " + channel->name() + "\r\n";
	client.appendToSendBuffer(successMsg);
	server.setPollout(fd, true);
}
