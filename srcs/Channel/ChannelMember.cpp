#include "Channel.hpp"
#include "Client.hpp"

void Channel::addMember(Client* client, bool isOperator)
{
	_members[client] = isOperator;
}

void Channel::removeMember(Client* client)
{
	_members.erase(client);
}

bool Channel::hasMember(Client* client) const
{
	return _members.find(client) != _members.end();
}

bool Channel::isOperator(Client* client) const
{
	std::map<Client*, bool>::const_iterator it = _members.find(client);
	if (it == _members.end())
		return false;
	return it->second;
}

void Channel::setOperator(Client* client, bool val)
{
	std::map<Client*, bool>::iterator it = _members.find(client);
	if (it != _members.end())
		it->second = val;
}

std::size_t Channel::memberCount() const
{
	return _members.size();
}

const std::map<Client*, bool>& Channel::members() const
{
	return _members;
}

void Channel::broadcast(const std::string& msg, Client* exclude)
{
	for (std::map<Client*, bool>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (it->first != exclude)
			it->first->queueSend(msg);
	}
}
