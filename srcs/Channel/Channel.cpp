#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel(const std::string& name)
	: _name(name), _topic(), _members(), _invited(), _modes()
{}

Channel::Channel(const Channel& other)
	: _name(other._name), _topic(other._topic),
	  _members(other._members), _invited(other._invited), _modes(other._modes)
{}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		_name    = other._name;
		_topic   = other._topic;
		_members = other._members;
		_invited = other._invited;
		_modes   = other._modes;
	}
	return *this;
}

Channel::~Channel() {}

const std::string& Channel::name() const        { return _name; }
const std::string& Channel::topic() const       { return _topic.text; }
const std::string& Channel::topicSetter() const { return _topic.setter; }

void Channel::setTopic(const std::string& topic, const std::string& setter)
{
	_topic.text   = topic;
	_topic.setter = setter;
}
