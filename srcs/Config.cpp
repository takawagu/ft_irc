#include "Config.hpp"

Config::Config(const std::string& port_str, const std::string& password): _port(parsePort(port_str)), _password(validatePassword(password)){}

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
	validatePortStr(str);
	int port_num = std::atoi(str.c_str());
	checkPortRange(port_num);
	return port_num;
}

void Config::validatePortStr(const std::string& str)
{
	if (str.empty())
		throw std::invalid_argument("port must not be empty");
	if (!isAllDigits(str))
		throw std::invalid_argument("port \"" + str + "\" must contain only digits");
	if (hasTooManyDigits(str))
		throw std::invalid_argument("port \"" + str + "\" is out of range (1-65535)");
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

bool Config::hasTooManyDigits(const std::string& str)
{
	return str.size() > 5;
}

void Config::checkPortRange(int port_num)
{
	if (port_num < MIN_PORT || port_num > MAX_PORT)
	{
		std::ostringstream oss;
		oss << "port " << port_num << " is out of range (" << MIN_PORT << "-" << MAX_PORT << ")";
		throw std::out_of_range(oss.str());
	}
}

const std::string& Config::validatePassword(const std::string& str)
{
	if (str.empty())
		throw std::invalid_argument("password must not be empty");

	for (std::string::size_type i = 0; i < str.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(str[i]);
		if (c == ' ')
			throw std::invalid_argument("password must not contain spaces");
		if (std::iscntrl(c))
			throw std::invalid_argument("password must not contain control characters");
	}

	return str;
}


