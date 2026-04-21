#include "Config.hpp"
#include "Server.hpp"

#include <iostream>

// int main(int argc, char** argv)
// {
//     if (argc != 3)
//     {
//         std::cerr << "Usage: ircserv <port> <password>" << std::endl;
//         return 1;
//     }

// 	try
// 	{
// 		Config config(argv[1], argv[2]);
// 		Server server(config);
// 		server.run();
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::cerr << "Error: " << e.what() << std::endl;
// 		return 1;
// 	}
// 	return 0;
// }

#include "Pass.hpp"
#include "Client.hpp"

int main(void){
	std::cout << "tochi test" << std::endl;
	ACommand* cmd = new Pass();
	Config config("6667", "password");
	Server server(config);
	Client* client = new Client(0,"tochi");

	cmd->execute(server,*client,0,"Pass password\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server,*client,0,"Pass \r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	cmd->execute(server,*client,0,"Pass pass word\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());

	// trim確認: \r\nあり・パスワード一致 → successなら trim OK、464なら \r が残っている
	std::cout << "--- trim test ---" << std::endl;
	cmd->execute(server,*client,0,"Pass password\r\n");
	std::string trimResult = client->sendBuffer();
	client->removeSentData(trimResult.length());
	if (trimResult.empty())
		std::cout << "[OK] \\r trimmed correctly" << std::endl;
	else
		std::cout << "[NG] \\r not trimmed: " << trimResult << std::endl;

	// 引数なし（スペースもない）→ 461
	std::cout << "--- no arg test ---" << std::endl;
	cmd->execute(server,*client,0,"Pass\r\n");
	std::cout << client->sendBuffer() << std::endl;
	client->removeSentData(client->sendBuffer().length());
}
