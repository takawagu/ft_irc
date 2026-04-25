#include "Channel.hpp"

bool Channel::isInviteOnly() const          { return _modes.invite_only; }
void Channel::setInviteOnly(bool val)       { _modes.invite_only = val; }

bool Channel::isInvited(Client* client) const
{
	return _invited.find(client) != _invited.end();
}

void Channel::addInvite(Client* client)    { _invited.insert(client); }
void Channel::removeInvite(Client* client) { _invited.erase(client); }

bool Channel::isTopicRestricted() const     { return _modes.topic_restricted; }
void Channel::setTopicRestricted(bool val)  { _modes.topic_restricted = val; }

const std::string& Channel::key() const     { return _modes.key; }
void Channel::setKey(const std::string& key)  { _modes.key = key; }

std::size_t Channel::userLimit() const              { return _modes.user_limit; }
void Channel::setUserLimit(std::size_t limit)       { _modes.user_limit = limit; }
