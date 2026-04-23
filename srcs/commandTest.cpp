#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Pass.hpp"
#include "Nick.hpp"

#include <iostream>

void PassTest();
void trimTest();
void NickTest();

void commandTest(int argc, char** argv)
{
	(void)argc;
	(void)argv; 
	// trimTest();
	// PassTest();
	NickTest();
}

void PassTest(){
	std::cout << "<<Pass test>>" << std::endl;
	ACommand* cmd = new Pass();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");

	cmd->execute(server, *client, 0, "Pass password\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server, *client, 0, "Pass \r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server, *client, 0, "Pass pass word\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());
}

void NickTest(){
	std::cout << "<<Nick test>>" << std::endl;
	ACommand* cmd = new Nick();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");

	cmd->execute(server, *client, 0, "Nick\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server, *client, 0, "Nick tochi\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	//　first char is only letter
	cmd->execute(server, *client, 0, "Nick 123number\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// ! is not allowed
	cmd->execute(server, *client, 0, "Nick tochi!\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// length <= 9
	cmd->execute(server, *client, 0, "Nick T123456789\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// same nickname is not allowed
	Client* client2 = new Client(0, "tochi2");
	cmd->execute(server, *client2, 0, "Nick tochi\r\n");
	std::cout << client2->sendBuffer() << std::endl;
	client2->removeSentData(client2->sendBuffer().length());

	std::cout << "Client 1 nickname: " << client->nickname() << std::endl;
	std::cout << "Client 2 nickname: " << client2->nickname() << std::endl;
}

void trimTest(){
	std::cout << "trim test" << std::endl;
	ACommand* cmd = new Pass();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0, "tochi");

	// trim確認: \r\nあり・パスワード一致 → successなら trim OK、464なら \r が残っている
	std::cout << "--- trim test ---" << std::endl;
	cmd->execute(server, *client, 0, "Pass password\r\n");
	std::string trimResult = client->sendBuffer();
	client->removeSentData(trimResult.length());
	if (trimResult.empty())
		std::cout << "[OK] \\r trimmed correctly" << std::endl;
	else
		std::cout << "[NG] \\r not trimmed: " << trimResult << std::endl;

	// 引数なし（スペースもない）→ 461
	std::cout << "--- no arg test ---" << std::endl;
	cmd->execute(server, *client, 0, "Pass\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());
}
