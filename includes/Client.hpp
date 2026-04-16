#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
	public:
		explicit Client(int fd, const std::string& hostname);
		Client(const Client&);
		Client& operator=(const Client&);
		~Client();

		int					fd() const;
		const std::string&	hostname() const;
		const std::string&	nickname() const;
		const std::string&	username() const;

		void				setNickname(const std::string& nick);
		void				setUsername(const std::string& user);

		void				appendRecv(const char* data, std::size_t len);
		bool				extractLine(std::string& out);

		bool				isRegistered() const;
		void				setRegistered(bool val);

		bool				isRecvBufferFull() const;

		void				queueSend(const std::string& msg);
		const std::string&	sendBuffer() const;
		void				eraseSent(std::size_t n);
		bool				hasPendingSend() const;

	private:
		int			_fd;
		std::string	_hostname;
		std::string	_nickname;
		std::string	_username;
		bool		_registered;
		std::string	_recv_buffer;
		std::string	_send_buffer;

		enum { MAX_RECV_BUFFER = 4096 * 4 };
};

#endif
