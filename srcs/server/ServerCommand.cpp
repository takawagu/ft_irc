#include "Server.hpp"
#include "Client.hpp"
#include "ACommand.hpp"
#include "Commands.hpp"

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

void Server::handleCommand(Client& client, int fd, const std::string& command, const std::string& params)
{
	std::map<std::string, ACommand*>::iterator it = _cmd_map.find(command);
	if (it == _cmd_map.end())
		return;

	// TODO: 未登録クライアントには PASS/NICK/USER/QUIT のみ許可
	// if (!client.isRegistered() && command != "PASS" && command != "NICK" && command != "USER" && command != "QUIT")
	//     return;

	it->second->execute(*this, client, fd, params);
}

void Server::deleteCommandMap()
{
	for (std::map<std::string, ACommand*>::iterator it = _cmd_map.begin(); it != _cmd_map.end(); ++it)
		delete it->second;
	_cmd_map.clear();
}
