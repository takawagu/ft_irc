#include "Nick.hpp"
#include "Server.hpp"
#include "Client.hpp"

bool isNicknameValid(const std::string& nick);

void Nick::executeAction(Server& server, Client& client, int fd)
{
	if (params().size() <= 1)
	{
		server.sendError(client, fd, "431", ":No nickname given");
		return;
	}
	if (!isNicknameValid(params()[1]))
	{
		server.sendError(client, fd, "432", params()[1] + " :Erroneus nickname");
		return;
	}
	if (server.isNicknameTaken(params()[1]))
	{
		server.sendError(client, fd, "433", params()[1] + " :Nickname is already in use");
		return;
	}
	std::string old_nick = client.nickname();
	client.setNickname(params()[1]);
	client.appendToSendBuffer(":" + old_nick + " NICK " + client.nickname() + "\r\n");
	return;
}

static bool isNicknameSymbol(char c);

bool isNicknameValid(const std::string& nick)
{
	if (nick.empty() || nick.length() > MAX_NICKNAME_LENGTH)
		return false;
	if (!isalpha(nick[0]))
		return false;
	for (size_t i = 1; i < nick.length(); ++i)
	{
		if (!isalnum(nick[i]) && isNicknameSymbol(nick[i]))
			return false;
	}
	return true;
}

static bool isNicknameSymbol(char c)
{
	return c == '-' || c == '_' || c == '[' || c == ']' || c == '\\' || c == '^' || c == '{' || c == '}' || c == '|';
}

// 最大9文字(MAX_NICKNAME_LENGTH)
// 先頭は英字
// 以降は英数字 + 一部記号
// スペースは禁止
// 大文字小文字は特殊ルールあり（{ = [ など）
// ネットワーク内でユニーク必須
