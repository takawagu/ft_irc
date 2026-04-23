#include "Client.hpp"

Client::Client(int fd, const std::string& hostname)
	: _fd(fd)
	, _hostname(hostname)
	, _nickname("*")
	, _username()
	, _realname()
	, _registered(false)
	, _pass_accepted(false)
	, _nick_set(false)
	, _user_set(false)
	, _recv_buffer()
	, _send_buffer()
	, _joined_channels()
{
}

Client::~Client() {}

int Client::fd() const
{
	return _fd;
}

const std::string& Client::hostname() const
{
	return _hostname;
}

const std::string& Client::nickname() const
{
	return _nickname;
}

const std::string& Client::username() const
{
	return _username;
}

void Client::setNickname(const std::string& nick)
{
	_nickname = nick;
	_nick_set = true;
}

void Client::setUsername(const std::string& user)
{
	_username = user;
	_user_set = true;
}

const std::string& Client::realname() const
{
	return _realname;
}

void Client::setRealname(const std::string& real)
{
	_realname = real;
}

bool Client::isPassAccepted() const
{
	return _pass_accepted;
}

void Client::setPassAccepted()
{
	_pass_accepted = true;
}

bool Client::isNickSet() const
{
	return _nick_set;
}

bool Client::isUserSet() const
{
	return _user_set;
}

bool Client::tryRegister()
{
	if (_pass_accepted && _nick_set && _user_set){
		_registered = true;
		return true;
	}
	return false;
}

void Client::joinChannel(const std::string& channel)
{
	_joined_channels.insert(channel);
}

void Client::leaveChannel(const std::string& channel)
{
	_joined_channels.erase(channel);
}

bool Client::isOnChannel(const std::string& channel) const
{
	return _joined_channels.find(channel) != _joined_channels.end();
}

const std::set<std::string>& Client::joinedChannels() const
{
	return _joined_channels;
}

bool Client::isRegistered() const
{
	return _registered;
}

void Client::setRegistered(bool val)
{
	_registered = val;
}

void Client::appendToRecvBuffer(const char* data, std::size_t len)
{
	_recv_buffer.append(data, len);
}

bool Client::extractLine(std::string& out)
{
	std::string::size_type pos = _recv_buffer.find('\n');
	if (pos == std::string::npos)
		return false;

	out = _recv_buffer.substr(0, pos);
	if (!out.empty() && out[out.size() - 1] == '\r')
		out.erase(out.size() - 1);

	_recv_buffer.erase(0, pos + 1);
	return true;
}

bool Client::isRecvBufferFull() const
{
	return _recv_buffer.size() > MAX_RECV_BUFFER;
}

void Client::appendToSendBuffer(const std::string& msg)
{
	_send_buffer += msg;
}

const std::string& Client::sendBuffer() const
{
	return _send_buffer;
}

void Client::removeSentData(std::size_t n)
{
	_send_buffer.erase(0, n);
}

bool Client::hasDataToSend() const
{
	return !_send_buffer.empty();
}
