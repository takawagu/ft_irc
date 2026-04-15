#include "Config.hpp"

Config::Config(int argc, char** argv): _port(0), _password()
{
	if (argc != 3)
		throw std::invalid_argument("usage: ircserv <port> <password>");

	_port     = parsePort(argv[1]);
	_password = argv[2];

	if (_password.empty())
		throw std::invalid_argument("password must not be empty");
}

Config::~Config() {}

int Config::port() const
{
	return _port;
}

const std::string& Config::password() const
{
	return _password;
}

int Config::parsePort(const std::string& str)
{
	if (str.empty())
		throw std::invalid_argument("port must not be empty");
	if (!isAllDigits(str))
		throw std::invalid_argument("port \"" + str + "\" must contain only digits");
	char* end = 0;
	long port_num = std::strtol(str.c_str(), &end, BASE_10);
	if (end == str.c_str() || *end != '\0')
		throw std::invalid_argument("port \"" + str + "\" is not a valid integer");
	checkPortRange(port_num);
	return static_cast<int>(port_num);
}

void Config::checkPortRange(long port_num)
{
	if (port_num < MIN_PORT || port_num > MAX_PORT)
	{
		std::ostringstream oss;
		oss << "port " << port_num << " is out of range (" << MIN_PORT << "-" << MAX_PORT << ")";
		throw std::out_of_range(oss.str());
	}
}

bool Config::isAllDigits(const std::string& str)
{
	if (str.empty())
		return false;
	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			return false;
	}
	return true;
}
