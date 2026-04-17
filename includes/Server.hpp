#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <string>
#include <vector>

#include <netinet/in.h>
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

		void	addToDisconnectList(int fd);
		void	setPollout(int fd, bool enable);

	private:
		static volatile sig_atomic_t	_stop;
		static void						signalHandler(int sig);

		const Config&					_config;
		int								_listen_fd;
		std::vector<pollfd>				_pfds;
		std::map<int, Client*>			_clients;
		std::vector<int>				_disconnect_list;
		std::map<std::string, ACommand*>	_cmd_map;

		void	setup();
		struct sockaddr_in	createListenAddr() const;
		void				registerPfd(int fd, short events);
		void				unregisterPfd(int fd);
		void	loop();
		void	waitForEvents();
		void	setupSignals();
		void	handlePollEvents();
		void	handleClientEvents(int fd, short revents);
		bool	isNewConnection(int fd, short revents) const;
		void	shutdown();
		void	closeAllClients();
		void	closeListenFd();

		void	initCommandMap();
		void	handleCommand(Client& client, int fd, const std::string& command, const std::string& params);

		void	acceptNewClient();
		int		acceptConnection(struct sockaddr_in& addr);
		void	registerClient(int cfd, const struct sockaddr_in& addr);
		void	handleClientRead(int fd);
		Client*	findClient(int fd);
		bool	handleRecvResult(ssize_t n, int fd);
		bool	checkRecvBufferOverflow(Client& client, int fd);
		void	processClientCommands(Client& client, int fd);
		bool	parseRequest(const std::string& line, std::string& command, std::string& params);
		void	handleClientWrite(int fd);
		bool	handleSendResult(ssize_t n, int fd);
		void	deleteCommandMap();
		void	deleteDisconnectedClients();
		void	removeClient(int fd);
};

#endif
