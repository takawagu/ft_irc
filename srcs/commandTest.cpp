#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Pass.hpp"
#include "Nick.hpp"
#include "User.hpp"
#include "Privmsg.hpp"
#include "Join.hpp"
#include "Topic.hpp"

#include <iostream>

void PassTest();
void trimTest();
void NickTest();
void RegisterTest();
void PrivmsgTest();
void JoinTest();
void topicTest();

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
	// JoinTest();
	// PrivmsgTest();
	topicTest();
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

static Client* makeRegisteredClient(Server& server, int fd, const std::string& nick)
{
	Client* client = new Client(fd, "localhost");
	server.addClient(fd, client);

	ACommand* pass = new Pass();
	pass->execute(server, *client, fd, makeParams("password"));
	client->removeSentData(client->sendBuffer().length());
	delete pass;

	ACommand* nick_cmd = new Nick();
	nick_cmd->execute(server, *client, fd, makeParams(nick));
	client->removeSentData(client->sendBuffer().length());
	delete nick_cmd;

	ACommand* user_cmd = new User();
	user_cmd->execute(server, *client, fd, makeParams("user1", "0", "*", "Real Name"));
	client->removeSentData(client->sendBuffer().length());
	delete user_cmd;

	return client;
}

static void printAndFlush(Client* client, const std::string& label)
{
	std::cout << "[" << label << "] " << client->sendBuffer();
	client->removeSentData(client->sendBuffer().length());
}

void PrivmsgTest()
{
	std::cout << "<<Privmsg test>>" << std::endl;
	Config config("6667", "password");
	Server server(config);
	Client* alice = makeRegisteredClient(server, 100, "alice");
	Client* bob = makeRegisteredClient(server, 101, "bob");

	ACommand* privmsg = new Privmsg();

	{
		std::cout << "-- no params --" << std::endl;
		privmsg->execute(server, *alice, 100, makeParams());
		printAndFlush(alice, "461 expected");
	}
	{
		std::cout << "-- no text --" << std::endl;
		privmsg->execute(server, *alice, 100, makeParams("bob"));
		printAndFlush(alice, "412 expected");
	}
	{
		std::cout << "-- no receiver --" << std::endl;
		privmsg->execute(server, *alice, 100, makeParams(":Hello"));
		printAndFlush(alice, "411 expected");
	}
	{
		std::cout << "-- too many receivers --" << std::endl;
		privmsg->execute(server, *alice, 100, makeParams("bob,#general", ":Hello"));
		printAndFlush(alice, "too many receivers");
	}
	{
		std::cout << "-- non-existing user --" << std::endl;
		privmsg->execute(server, *alice, 100, makeParams("charlie", ":Hello"));
		printAndFlush(alice, "403 expected");
	}
	{
		std::cout << "-- non-existing channel --" << std::endl;
		privmsg->execute(server, *alice, 100, makeParams("#nonexistent", ":Hello"));
		printAndFlush(alice, "403 expected");
	}
	{
		std::cout << "-- normal channel message --" << std::endl;
		Channel* ch = server.getOrCreateChannel("#general");
		ch->addMember(alice);
		privmsg->execute(server, *alice, 100, makeParams("#general", ":Hello everyone!"));
		printAndFlush(bob, "[channel] Hello everyone! expected");

		// ユーザ宛 → 正常送信
		privmsg->execute(server, *alice, 100, makeParams("bob", ":Hello Bob!"));
		printAndFlush(bob, "[private] Hello Bob! expected");
	}
}

void JoinTest()
{
	std::cout << "<<Join test>>" << std::endl;
	Config config("6667", "password");
	ACommand* join = new Join();

	// パラメータなし → 461 Not enough parameters
	{
		std::cout << "-- no params --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams());
		printAndFlush(client, "461 expected");
	}

	// 無効なチャンネル名（#なし）→ 403 No such channel
	{
		std::cout << "-- invalid channel name (no #) --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams("nochannel"));
		printAndFlush(client, "403 expected");
	}

	// 無効なチャンネル名（#のみ）→ 403 No such channel
	{
		std::cout << "-- invalid channel name (# only) --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams("#"));
		printAndFlush(client, "403 expected");
	}

	// 正常JOIN（トピックなし）→ JOIN + 331 + 353 + 366
	{
		std::cout << "-- normal join (no topic) --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams("#general"));
		printAndFlush(client, "JOIN+331+353+366 expected");
	}

	// 正常JOIN（トピックあり）→ JOIN + 332 + 353 + 366
	{
		std::cout << "-- normal join (with topic) --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#general"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#general");
		ch->setTopic("hello world", "alice");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#general"));
		printAndFlush(bob, "JOIN+332+353+366 expected");
	}

	// すでに参加済み → 何も送信されない
	{
		std::cout << "-- already joined --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams("#general"));
		client->removeSentData(client->sendBuffer().length());
		join->execute(server, *client, 100, makeParams("#general"));
		std::string buf = client->sendBuffer();
		if (buf.empty())
			std::cout << "[already joined] OK: no message sent" << std::endl;
		else
			std::cout << "[already joined] NG: " << buf;
		client->removeSentData(buf.length());
	}

	// invite-only + 招待なし → 473
	{
		std::cout << "-- invite-only, not invited --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#secret"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#secret");
		ch->setInviteOnly(true);

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#secret"));
		printAndFlush(bob, "473 expected");
	}

	// invite-only + 招待あり → 正常JOIN
	{
		std::cout << "-- invite-only, invited --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#secret"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#secret");
		ch->setInviteOnly(true);
		ch->addInvite("bob");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#secret"));
		printAndFlush(bob, "JOIN expected");
	}

	// キー違い → 475
	{
		std::cout << "-- wrong key --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#keyed"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#keyed");
		ch->setKey("secret");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#keyed", "wrong"));
		printAndFlush(bob, "475 expected");
	}

	// キー正しい → 正常JOIN
	{
		std::cout << "-- correct key --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#keyed"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#keyed");
		ch->setKey("secret");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#keyed", "secret"));
		printAndFlush(bob, "JOIN expected");
	}

	// ユーザーリミット超過 → 471
	{
		std::cout << "-- user limit exceeded --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#limited"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#limited");
		ch->setUserLimit(1);

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#limited"));
		printAndFlush(bob, "471 expected");
	}

	// 複数チャンネルへの同時JOIN
	{
		std::cout << "-- multi channel join --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#chan1,#chan2,#chan3"));
		printAndFlush(alice, "3x JOIN expected");
	}

	// 複数チャンネル + 対応するキー
	{
		std::cout << "-- multi channel join with keys --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#a,#b"));
		alice->removeSentData(alice->sendBuffer().length());
		server.findChannel("#a")->setKey("ka");
		server.findChannel("#b")->setKey("kb");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#a,#b", "ka,kb"));
		printAndFlush(bob, "2x JOIN expected");
	}

	// JOINしたとき他のメンバーにブロードキャストされる
	{
		std::cout << "-- broadcast to existing members --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#general"));
		alice->removeSentData(alice->sendBuffer().length());

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#general"));
		std::string aliceBuf = alice->sendBuffer();
		if (aliceBuf.find("bob") != std::string::npos)
			std::cout << "[broadcast] OK: alice received bob's JOIN" << std::endl;
		else
			std::cout << "[broadcast] NG: alice did not receive bob's JOIN" << std::endl;
		alice->removeSentData(aliceBuf.length());
	}

	// #以外のプレフィックス → 403 No such channel
	{
		std::cout << "-- invalid prefix (&channel) --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams("&general"));
		printAndFlush(client, "403 expected");
	}

	// チャンネル名にスペース → 403 No such channel
	{
		std::cout << "-- channel name with space --" << std::endl;
		Server server(config);
		Client* client = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *client, 100, makeParams("#chan nel"));
		printAndFlush(client, "403 expected");
	}

	// invite-only: JOIN後にinvite-listから削除される → 再JOINは通常どおり弾かれる
	{
		std::cout << "-- invite removed after join --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#secret"));
		alice->removeSentData(alice->sendBuffer().length());
		Channel* ch = server.findChannel("#secret");
		ch->setInviteOnly(true);
		ch->addInvite("bob");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#secret"));
		bob->removeSentData(bob->sendBuffer().length());

		if (!ch->isInvited("bob"))
			std::cout << "[invite removed] OK: invite cleared after join" << std::endl;
		else
			std::cout << "[invite removed] NG: invite still present after join" << std::endl;
	}

	// キー数がチャンネル数より少ない → 不足分は空文字キーで照合
	{
		std::cout << "-- fewer keys than channels --" << std::endl;
		Server server(config);
		Client* alice = makeRegisteredClient(server, 100, "alice");
		join->execute(server, *alice, 100, makeParams("#a,#b"));
		alice->removeSentData(alice->sendBuffer().length());
		server.findChannel("#a")->setKey("ka");
		server.findChannel("#b")->setKey("kb");

		Client* bob = makeRegisteredClient(server, 101, "bob");
		join->execute(server, *bob, 101, makeParams("#a,#b", "ka"));
		std::string buf = bob->sendBuffer();
		if (buf.find(":bob!user1@localhost JOIN #a") != std::string::npos)
			std::cout << "[fewer keys] OK: joined #a with correct key" << std::endl;
		else
			std::cout << "[fewer keys] NG: did not join #a" << std::endl;
		if (buf.find("475") != std::string::npos)
			std::cout << "[fewer keys] OK: blocked from #b (missing key)" << std::endl;
		else
			std::cout << "[fewer keys] NG: should have been blocked from #b" << std::endl;
		bob->removeSentData(buf.length());
	}

	delete join;
}

void topicTest()
{
	std::cout << "<<Topic test>>" << std::endl;
	Config config("6667", "password");
	Server server(config);
	Client* alice = makeRegisteredClient(server, 100, "alice");
	Client* bob = makeRegisteredClient(server, 101, "bob");

	Channel* ch = server.getOrCreateChannel("#general");
	ch->addMember(alice);
	ch->addMember(bob);

	ACommand* topic = new Topic();

	// パラメータなし → 461 Not enough parameters
	{
		std::cout << "-- no params --" << std::endl;
		topic->execute(server, *alice, 100, makeParams());
		printAndFlush(alice, "461 expected");
	}

	// チャンネル名のみ → トピック表示（トピックなし）
	{
		std::cout << "-- channel name only (no topic) --" << std::endl;
		topic->execute(server, *alice, 100, makeParams("#general"));
		printAndFlush(alice, "331 expected");
	}

	// チャンネル名のみ → トピック表示（トピックあり）
	{
		std::cout << "-- channel name only (with topic) --" << std::endl;
		ch->setTopic("hello world", "alice");
		topic->execute(server, *alice, 100, makeParams("#general"));
		printAndFlush(alice, "332 expected");
	}

	// チャンネル名 + 新しいトピック → トピック変更
	{
		std::cout << "-- channel name + new topic --" << std::endl;
		topic->execute(server, *alice, 100, makeParams("#general", ":new topic"));
		printAndFlush(alice, "topic changed expected (alice)");
		printAndFlush(bob, "topic changed expected (bob)");
	}

	delete topic;
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
