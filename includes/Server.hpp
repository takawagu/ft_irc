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

		static int	checkSyscall(int ret, const char* name);

		void	setup();
		struct sockaddr_in	createListenAddr() const;
		void				registerPfd(int fd, short events);
		void				registerListenPfd();
		void	loop();
		void	setupSignals();
		void	handlePollEvents();
		void	handleClientEvents(int fd, short revents);
		bool	isNewConnection(int fd, short revents) const;
		static bool	isReadable(short revents);
		static bool	isWritable(short revents);
		static bool	hasError(short revents);
		static bool	isDisconnected(short revents);
		void	shutdown();

		void	initCommandMap();
		void	handleCommand(Client& client, int fd, const std::string& command, const std::string& params);

		void	acceptNewClient();
		int		acceptConnection(struct sockaddr_in& addr);
		void	registerClient(int cfd, const struct sockaddr_in& addr);
		void	handleClientRead(int fd);
		void	parseLine(Client& client, int fd, const std::string& line);
		void	handleClientWrite(int fd);
		void	deleteCommandMap();
		void	deleteDisconnectedClients();
		void	removeClient(int fd);
};

#endif
