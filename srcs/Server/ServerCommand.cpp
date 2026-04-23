#include "Server.hpp"
#include "Client.hpp"
#include "ACommand.hpp"
#include "Commands.hpp"

#include <iostream>

static bool isAllowedBeforeRegistration(const std::string& command);
static std::string extractCommand(const std::string& line, std::string::size_type& pos);
static void parseParams(const std::string& line, std::string::size_type pos, std::vector<std::string>& params);
static void skipSpaces(const std::string& line, std::string::size_type& pos);


void Server::initCommandMap()
{
	_cmd_map["PASS"]    = new Pass();
	_cmd_map["NICK"]    = new Nick();
	_cmd_map["USER"]    = new User();
	_cmd_map["PRIVMSG"] = new Privmsg();
	_cmd_map["JOIN"]    = new Join();
	_cmd_map["KICK"]    = new Kick();
	_cmd_map["INVITE"]  = new Invite();
	_cmd_map["TOPIC"]   = new Topic();
	_cmd_map["MODE"]    = new Mode();
	_cmd_map["PING"]    = new Ping();
	_cmd_map["PONG"]    = new Pong();
	_cmd_map["QUIT"]    = new Quit();
	_cmd_map["PART"]    = new Part();
}

void Server::handleCommand(Client& client, int fd, const std::string& command, const std::vector<std::string>& params)
{
	ACommand* cmd = findInCmdMap(command);
	if (cmd == NULL)
		return;

	if (!client.isRegistered() && !isAllowedBeforeRegistration(command))
		return;

	cmd->execute(*this, client, fd, params);
}

ACommand* Server::findInCmdMap(const std::string& command) const
{
	std::map<std::string, ACommand*>::const_iterator it = _cmd_map.find(command);
	if (it == _cmd_map.end())
		return NULL;
	return it->second;
}

static bool isAllowedBeforeRegistration(const std::string& command)
{
	return command == "PASS" || command == "NICK"
		|| command == "USER" || command == "QUIT";
}

void Server::deleteCommandMap()
{
	for (std::map<std::string, ACommand*>::iterator it = _cmd_map.begin(); it != _cmd_map.end(); ++it)
		delete it->second;
	_cmd_map.clear();
}

bool Server::parseRequest(const std::string& line, std::string& command, std::vector<std::string>& params)
{
	std::cout << ">> " << line << std::endl;

	std::string::size_type pos = 0;
	command = extractCommand(line, pos);
	if (command.empty())
		return false;

	params.clear();
	parseParams(line, pos, params);
	return true;
}

static std::string extractCommand(const std::string& line, std::string::size_type& pos)
{
	skipSpaces(line, pos);
	if (pos == line.size())
		return std::string();

	std::string::size_type end = line.find(' ', pos);
	if (end == std::string::npos)
		end = line.size();

	std::string command = line.substr(pos, end - pos);
	pos = end;
	return command;
}

// IRC RFC 準拠: ':' で始まるトークンはそれ以降を trailing として1つのパラメータにまとめる
static void parseParams(const std::string& line, std::string::size_type pos, std::vector<std::string>& params)
{
	while (pos < line.size())
	{
		skipSpaces(line, pos);
		if (pos >= line.size())
			break;

		if (line[pos] == ':')
		{
			params.push_back(line.substr(pos + 1));
			break;
		}

		std::string::size_type end = line.find(' ', pos);
		if (end == std::string::npos)
			end = line.size();

		params.push_back(line.substr(pos, end - pos));
		pos = end;
	}
}

static void skipSpaces(const std::string& line, std::string::size_type& pos)
{
	while (pos < line.size() && line[pos] == ' ')
		++pos;
}
