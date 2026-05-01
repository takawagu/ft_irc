#include "Topic.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Topic::executeAction(Server& server, Client& client, int fd)
{
	if (params().empty() || params()[0][0] == ':')
	{
		server.sendError(client, fd, "461", "TOPIC :Not enough parameters");
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
	if (params().size() == 1)
	{
		std::string topicReply;
		if (!channel->topic().empty())
			topicReply = ":ircserv 332 " + client.nickname() + " " + channel->name() + " :" + channel->topic() + "\r\n";
		else
			topicReply = ":ircserv 331 " + client.nickname() + " " + channel->name() + " :No topic is set\r\n";
		client.appendToSendBuffer(topicReply);
		server.setPollout(fd, true);
		return;
	}
	else
	{
		if (channel->isTopicRestricted() && !channel->isOperator(&client))
		{
			server.sendError(client, fd, "482", params()[0] + " :You're not channel operator");
			return;
		}
		std::string topic = params()[1];
		if (!topic.empty() && topic[0] == ':')
			topic = topic.substr(1);
		channel->setTopic(topic, client.nickname());
		std::string topicChangeMsg = ":" + client.prefix() + " TOPIC " + channel->name() + " :" + channel->topic() + "\r\n";
		server.broadcastToChannel(channel, topicChangeMsg);
	}
	return;
}
