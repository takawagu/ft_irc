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
class Channel;

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
		void	sendError(Client& client, int fd, const std::string& code, const std::string& body);

		Channel*	findChannel(const std::string& name) const;
		Channel*	getOrCreateChannel(const std::string& name);
		void		deleteChannelIfEmpty(const std::string& name);
		void		broadcastToChannel(Channel* channel, const std::string& msg, Client* exclude = NULL);
		Client*		findClientByNick(const std::string& nick) const;

		bool checkPassword(const std::string& pass) const;
		bool isNicknameTaken(const std::string& nick) const;
		std::string toScandanavianLower(const std::string& nick) const;
		void addClient(int fd, Client* client);

	private:
		static volatile sig_atomic_t	_stop;
		static void						signalHandler(int sig);

		const Config&					_config;
		int								_listen_fd;
		std::vector<pollfd>				_pfds;
		std::map<int, Client*>			_clients;
		std::vector<int>				_disconnect_list;
		std::map<std::string, ACommand*>	_cmd_map;
		std::map<std::string, Channel*>		_channels;

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
		void	closeAllChannels();
		void	closeAllClients();
		void	closeListenFd();

		void		initCommandMap();
		ACommand*	findInCmdMap(const std::string& command) const;
		void		handleCommand(Client& client, int fd, const std::string& command, const std::vector<std::string>& params);

		void	acceptNewClient();
		int		acceptConnection(struct sockaddr_in& addr);
		bool	isAcceptSuccessful(int client_fd);
		bool	setNonBlocking(int fd);
		void	registerClient(int client_fd, const struct sockaddr_in& addr);
		void	handleClientRead(int fd);
		Client*	findClient(int fd);
		bool	isRecvSuccessful(ssize_t bytesReceived, int fd);
		bool	isRecvBufferAvailable(Client& client, int fd);
		void	processClientRequests(Client& client, int fd);
		bool	parseRequest(const std::string& line, std::string& command, std::vector<std::string>& params);
		void	handleClientWrite(int fd);
		bool	isSendSuccessful(ssize_t bytesSent, int fd);
		void	deleteCommandMap();
		void	deleteDisconnectedClients();
		void	removeClient(int fd);
};

#endif
