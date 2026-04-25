#include "Join.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandUtils.hpp"

static void expandChannelsAndKeys(const std::vector<std::string>& params, std::vector<std::string>& channels, std::vector<std::string>& keys);
static bool isJoinBlocked(Server& server, Client& client, int fd, Channel* channel, const std::string& channelName, const std::string& key);
static Channel* addClientToChannel(Server& server, Client& client, const std::string& channelName, bool isNewChannel);
static void sendJoinReplies(Server& server, Client& client, int fd, Channel* channel, const std::string& channelName);
static void broadcastJoin(Server& server, Client& client, Channel* channel, const std::string& channelName);
static void sendTopic(Client& client, Channel* channel, const std::string& channelName);
static void sendNamesList(Client& client, Channel* channel, const std::string& channelName);
static void joinOneChannel(Server& server, Client& client, int fd, const std::string& channelName, const std::string& key);

void Join::executeAction(Server& server, Client& client, int fd)
{
	if (params().empty())
	{
		server.sendError(client, fd, "461", "JOIN :Not enough parameters");
		return;
	}

	std::vector<std::string> channels;
	std::vector<std::string> keys;
	expandChannelsAndKeys(params(), channels, keys);

	for (size_t i = 0; i < channels.size(); i++)
	{
		std::string key;
		if (i < keys.size())
			key = keys[i];
		joinOneChannel(server, client, fd, channels[i], key);
	}
}

static void joinOneChannel(Server& server, Client& client, int fd, const std::string& channelName, const std::string& key)
{
	if (!isChannelNameValid(channelName))
	{
		server.sendError(client, fd, "403", channelName + " :No such channel");
		return;
	}

	Channel* channel = server.findChannel(channelName);
	bool isNewChannel = (channel == NULL);

	if (!isNewChannel && isJoinBlocked(server, client, fd, channel, channelName, key))
		return;

	channel = addClientToChannel(server, client, channelName, isNewChannel);
	sendJoinReplies(server, client, fd, channel, channelName);
}

static bool isJoinBlocked(Server& server, Client& client, int fd, Channel* channel, const std::string& channelName, const std::string& key)
{
	if (channel->hasMember(&client))
		return true;
	if (channel->isInviteOnly() && !channel->isInvited(&client))
	{
		server.sendError(client, fd, "473", channelName + " :Cannot join channel (+i)");
		return true;
	}
	if (!channel->key().empty() && channel->key() != key)
	{
		server.sendError(client, fd, "475", channelName + " :Cannot join channel (+k)");
		return true;
	}
	if (channel->userLimit() > 0 && channel->memberCount() >= channel->userLimit())
	{
		server.sendError(client, fd, "471", channelName + " :Cannot join channel (+l)");
		return true;
	}
	return false;
}

static Channel* addClientToChannel(Server& server, Client& client, const std::string& channelName, bool isNewChannel)
{
	Channel* channel = server.getOrCreateChannel(channelName);
	channel->addMember(&client, isNewChannel);
	client.joinChannel(channelName);
	if (channel->isInvited(&client))
		channel->removeInvite(&client);
	return channel;
}

static void sendJoinReplies(Server& server, Client& client, int fd, Channel* channel, const std::string& channelName)
{
	broadcastJoin(server, client, channel, channelName);
	sendTopic(client, channel, channelName);
	sendNamesList(client, channel, channelName);
	server.setPollout(fd, true);
}

static void broadcastJoin(Server& server, Client& client, Channel* channel, const std::string& channelName)
{
	std::string joinMsg = ":" + client.prefix() + " JOIN " + channelName + "\r\n";
	server.broadcastToChannel(channel, joinMsg);
}

static void sendTopic(Client& client, Channel* channel, const std::string& channelName)
{
	if (!channel->topic().empty())
	{
		std::string topicReply = ":ircserv 332 " + client.nickname() + " " + channelName + " :" + channel->topic() + "\r\n";
		client.appendToSendBuffer(topicReply);
	}
	else
	{
		std::string noTopicReply = ":ircserv 331 " + client.nickname() + " " + channelName + " :No topic is set\r\n";
		client.appendToSendBuffer(noTopicReply);
	}
}

static void sendNamesList(Client& client, Channel* channel, const std::string& channelName)
{
	std::string namesList = ":ircserv 353 " + client.nickname() + " = " + channelName + " :";
	const std::map<Client*, bool>& members = channel->members();
	for (std::map<Client*, bool>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (it != members.begin())
			namesList += " ";
		if (it->second)
			namesList += "@";
		namesList += it->first->nickname();
	}
	namesList += "\r\n";
	client.appendToSendBuffer(namesList);

	std::string endOfNames = ":ircserv 366 " + client.nickname() + " " + channelName + " :End of /NAMES list\r\n";
	client.appendToSendBuffer(endOfNames);
}

static void expandChannelsAndKeys(const std::vector<std::string>& params, std::vector<std::string>& channels, std::vector<std::string>& keys)
{
	channels = splitByComma(params[0]);
	if (params.size() >= 2)
		keys = splitByComma(params[1]);
}
