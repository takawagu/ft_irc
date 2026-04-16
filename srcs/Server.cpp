#include "Server.hpp"
#include "Client.hpp"
#include "Commands.hpp"

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

volatile sig_atomic_t Server::_stop = 0;

void Server::signalHandler(int)
{
	_stop = 1;
}

Server::Server(const Config& config): _config(config), _listen_fd(-1) {}

Server::~Server() {}

void Server::run()
{
	setup();
	loop();
	shutdown();
}

void Server::setup()
{
	initCommandMap();
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0)
		throw std::runtime_error("socket: " + std::string(std::strerror(errno)));

	int opt = 1;
	if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt: " + std::string(std::strerror(errno)));

	if (fcntl(_listen_fd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fcntl: " + std::string(std::strerror(errno)));

	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port        = htons(static_cast<uint16_t>(_config.port()));

	if (bind(_listen_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
		throw std::runtime_error("bind: " + std::string(std::strerror(errno)));

	if (listen(_listen_fd, SOMAXCONN) < 0)
		throw std::runtime_error("listen: " + std::string(std::strerror(errno)));

	pollfd lp;
	lp.fd      = _listen_fd;
	lp.events  = POLLIN;
	lp.revents = 0;
	_pfds.push_back(lp);

	std::cout << "Server listening on port " << _config.port() << std::endl;
}

void Server::loop()
{
	signal(SIGINT,  signalHandler);
	signal(SIGQUIT, signalHandler);
	signal(SIGPIPE, SIG_IGN);

	while (!_stop)
	{
		int ready = poll(&_pfds[0], _pfds.size(), -1);
		if (ready < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error("poll: " + std::string(std::strerror(errno)));
		}

		for (std::size_t i = 0; i < _pfds.size(); ++i)
		{
			short re = _pfds[i].revents;
			if (re == 0)
				continue;

			int fd = _pfds[i].fd;

			if (fd == _listen_fd)
			{
				if (re & POLLIN)
					acceptNewClient();
				continue;
			}

			if (re & (POLLERR | POLLNVAL))
			{
				disconnectClient(fd);
				continue;
			}
			if (re & POLLIN)
				handleClientRead(fd);
			if (re & POLLOUT)
				handleClientWrite(fd);
			if (re & POLLHUP)
				disconnectClient(fd);
		}

		flushPendingDisconnects();
	}
}

void Server::shutdown()
{
	deleteCommandMap();
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	_pfds.clear();

	if (_listen_fd >= 0)
	{
		close(_listen_fd);
		_listen_fd = -1;
	}

	std::cout << "Server shut down." << std::endl;
}

void Server::acceptNewClient()
{
	struct sockaddr_in cli_addr;
	socklen_t cli_len = sizeof(cli_addr);
	int cfd = accept(_listen_fd, reinterpret_cast<struct sockaddr*>(&cli_addr), &cli_len);
	if (cfd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
		return;
	}

	if (fcntl(cfd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl: " << std::strerror(errno) << std::endl;
		close(cfd);
		return;
	}

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &cli_addr.sin_addr, ip, sizeof(ip));

	Client* client = new Client(cfd, ip);
	_clients[cfd] = client;

	pollfd pfd;
	pfd.fd      = cfd;
	pfd.events  = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);

	std::cout << "Client connected: " << ip << " (fd " << cfd << ")" << std::endl;
}

void Server::handleClientRead(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client& client = *it->second;
	char buf[4096];
	ssize_t n = recv(fd, buf, sizeof(buf), 0);

	if (n <= 0)
	{
		disconnectClient(fd);
		return;
	}

	client.appendRecv(buf, static_cast<std::size_t>(n));

	if (client.isRecvBufferFull())
	{
		disconnectClient(fd);
		return;
	}

	std::string line;
	while (client.extractLine(line))
	{
		std::cout << "fd " << fd << " >> " << line << std::endl;

		std::string::size_type pos = 0;
		while (pos < line.size() && line[pos] == ' ')
			++pos;
		if (pos == line.size())
			continue;

		std::string::size_type cmd_end = line.find(' ', pos);
		if (cmd_end == std::string::npos)
			cmd_end = line.size();

		std::string command = line.substr(pos, cmd_end - pos);

		std::string::size_type arg_pos = cmd_end;
		while (arg_pos < line.size() && line[arg_pos] == ' ')
			++arg_pos;

		std::string params = (arg_pos < line.size()) ? line.substr(arg_pos) : std::string();

		handleCommand(client, fd, command, params);
	}
}

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

void Server::deleteCommandMap()
{
	for (std::map<std::string, ACommand*>::iterator it = _cmd_map.begin(); it != _cmd_map.end(); ++it)
		delete it->second;
	_cmd_map.clear();
}

void Server::handleCommand(Client& client, int fd, const std::string& command, const std::string& params)
{
	std::map<std::string, ACommand*>::iterator it = _cmd_map.find(command);
	if (it != _cmd_map.end())
		it->second->execute(*this, client, fd, params);
}

void Server::handleClientWrite(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	Client& client = *it->second;
	if (!client.hasPendingSend())
	{
		setPollout(fd, false);
		return;
	}

	ssize_t n = send(fd, client.sendBuffer().data(), client.sendBuffer().size(), 0);
	if (n <= 0)
	{
		disconnectClient(fd);
		return;
	}

	client.eraseSent(static_cast<std::size_t>(n));
	if (!client.hasPendingSend())
		setPollout(fd, false);
}

void Server::disconnectClient(int fd)
{
	_pending_disconnect.push_back(fd);
}

void Server::flushPendingDisconnects()
{
	for (std::size_t i = 0; i < _pending_disconnect.size(); ++i)
		removeClient(_pending_disconnect[i]);
	_pending_disconnect.clear();
}

void Server::removeClient(int fd)
{
	std::map<int, Client*>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;

	std::cout << "Client disconnected: " << it->second->hostname()
			  << " (fd " << fd << ")" << std::endl;

	close(fd);
	delete it->second;
	_clients.erase(it);

	for (std::size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			_pfds.erase(_pfds.begin() + static_cast<std::ptrdiff_t>(i));
			break;
		}
	}
}

void Server::setPollout(int fd, bool enable)
{
	for (std::size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			if (enable)
				_pfds[i].events |= POLLOUT;
			else
				_pfds[i].events &= ~POLLOUT;
			return;
		}
	}
}
