#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Pass.hpp"
#include "Nick.hpp"
#include "User.hpp"

#include <iostream>

void PassTest();
void trimTest();
void NickTest();
void RegisterTest();

static std::vector<std::string> makeParams()
{
	return std::vector<std::string>();
}

static std::vector<std::string> makeParams(const std::string& a)
{
	std::vector<std::string> v;
	v.push_back(a);
	return v;
}

static std::vector<std::string> makeParams(const std::string& a, const std::string& b)
{
	std::vector<std::string> v;
	v.push_back(a);
	v.push_back(b);
	return v;
}

static std::vector<std::string> makeParams(const std::string& a, const std::string& b,const std::string& c, const std::string& d)
{
	std::vector<std::string> v;
	v.push_back(a);
	v.push_back(b);
	v.push_back(c);
	v.push_back(d);
	return v;
}

void commandTest()
{
	// trimTest();
	// PassTest();
	// NickTest();
	RegisterTest();
}

void PassTest(){
	std::cout << "<<Pass test>>" << std::endl;
	ACommand* cmd = new Pass();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");

	cmd->execute(server, *client, 0, makeParams("password"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server, *client, 0, makeParams());
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server, *client, 0, makeParams("pass", "word"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());
}

void NickTest(){
	std::cout << "<<Nick test>>" << std::endl;
	ACommand* cmd = new Nick();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");
	server.addClient(0, client);

	cmd->execute(server, *client, 0, makeParams());
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server, *client, 0, makeParams("tochi"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	//　first char is only letter
	cmd->execute(server, *client, 0, makeParams("123number"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// ! is not allowed
	cmd->execute(server, *client, 0, makeParams("tochi!"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// length <= 9
	cmd->execute(server, *client, 0, makeParams("T123456789"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// same nickname is not allowed
	Client* client2 = new Client(1, "tochi2");
	server.addClient(1, client2);
	cmd->execute(server, *client2, 0, makeParams("tochi"));
	std::cout << client2->sendBuffer() << std::endl;
	client2->removeSentData(client2->sendBuffer().length());

	std::cout << "Client 1 nickname: " << client->nickname() << std::endl;
	std::cout << "Client 2 nickname: " << client2->nickname() << std::endl;

	// case-insensitive: Tochi == tochi
	Client* client3 = new Client(2, "tochi3");
	server.addClient(2, client3);
	cmd->execute(server, *client3, 2, makeParams("Tochi"));
	std::cout << client3->sendBuffer() << std::endl;
	client3->removeSentData(client3->sendBuffer().length());
	std::cout << "Client 3 nickname: " << client3->nickname() << std::endl;

	// scandinavian: T{} == T[]
	Client* client4 = new Client(3, "tochi4");
	server.addClient(3, client4);
	cmd->execute(server, *client4, 3, makeParams("T[]"));
	std::cout << client4->sendBuffer() << std::endl;
	client4->removeSentData(client4->sendBuffer().length());
	std::cout << "Client 4 nickname: " << client4->nickname() << std::endl;

	Client* client5 = new Client(4, "tochi5");
	server.addClient(4, client5);
	cmd->execute(server, *client5, 4, makeParams("T{}"));
	std::cout << client5->sendBuffer() << std::endl;
	client5->removeSentData(client5->sendBuffer().length());
	std::cout << "Client 5 nickname: " << client5->nickname() << std::endl;
}

void RegisterTest(){
	std::cout << "<<Register test>>" << std::endl;
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");

	ACommand* pass_cmd = new Pass();
	pass_cmd->execute(server, *client, 0, makeParams("password"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	ACommand* nick_cmd = new Nick();
	nick_cmd->execute(server, *client, 0, makeParams("tochi"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	ACommand* user_cmd = new User();
	user_cmd->execute(server, *client, 0, makeParams("tomo", "0", "*", "Tomo Ka"));
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());
}

void trimTest(){
	std::cout << "trim test" << std::endl;
	ACommand* cmd = new Pass();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");

	std::cout << "--- trim test ---" << std::endl;
	cmd->execute(server, *client, 0, makeParams("password"));
	std::string trimResult = client->sendBuffer();
	client->removeSentData(trimResult.length());
	if (trimResult.empty())
		std::cout << "[OK] \\r trimmed correctly" << std::endl;
	else
		std::cout << "[NG] \\r not trimmed: " << trimResult << std::endl;

	std::cout << "--- no arg test ---" << std::endl;
	cmd->execute(server, *client, 0, makeParams());
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	std::cout << "--- no arg test ---" << std::endl;
	cmd->execute(server, *client, 0, makeParams("tomo", "0", "*", "Tomo Ka"));
}
