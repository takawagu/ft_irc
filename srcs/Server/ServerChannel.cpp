#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

Channel* Server::findChannel(const std::string& name) const
{
	std::map<std::string, Channel*>::const_iterator it = _channels.find(name);
	if (it == _channels.end())
		return NULL;
	return it->second;
}

Channel* Server::getOrCreateChannel(const std::string& name)
{
	Channel* channel = findChannel(name);
	if (channel != NULL)
		return channel;

	channel = new Channel(name);
	_channels[name] = channel;
	return channel;
}

void Server::deleteChannelIfEmpty(const std::string& name)
{
	Channel* channel = findChannel(name);
	if (channel == NULL)
		return;

	if (channel->memberCount() == 0)
	{
		delete channel;
		_channels.erase(name);
	}
}

void Server::broadcastToChannel(Channel* channel, const std::string& msg, Client* exclude)
{
	const std::map<Client*, bool>& members = channel->members();
	for (std::map<Client*, bool>::const_iterator it = members.begin(); it != members.end(); ++it)
	{
		if (it->first != exclude)
		{
			it->first->queueSend(msg);
			setPollout(it->first->fd(), true);
		}
	}
}

Client* Server::findClientByNick(const std::string& nick) const
{
	for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->nickname() == nick)
			return it->second;
	}
	return NULL;
}
