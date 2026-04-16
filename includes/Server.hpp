#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>

#include <poll.h>
#include <signal.h>

#include "Config.hpp"

class Client;
class ACommand;

class Server
{
	public:
		explicit Server(const Config& config);
		Server(const Server&);
		Server& operator=(const Server&);
		~Server();

		void run();

		void	disconnectClient(int fd);
		void	setPollout(int fd, bool enable);

	private:
		static volatile sig_atomic_t	_stop;
		static void						signalHandler(int sig);

		const Config&					_config;
		int								_listen_fd;
		std::vector<pollfd>				_pfds;
		std::map<int, Client*>			_clients;
		std::vector<int>				_pending_disconnect;
		std::map<std::string, ACommand*>	_cmd_map;

		void	setup();
		void	loop();
		void	shutdown();

		void	initCommandMap();
		void	handleCommand(Client& client, int fd, const std::string& command, const std::string& params);

		void	acceptNewClient();
		void	handleClientRead(int fd);
		void	handleClientWrite(int fd);
		void	deleteCommandMap();
		void	flushPendingDisconnects();
		void	removeClient(int fd);
};

#endif
