#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>

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
		void				setNickname(const std::string& nick);
		const std::string&	username() const;
		void				setUsername(const std::string& user);
		const std::string&	realname() const;
		void				setRealname(const std::string& real);

		bool				isPassAccepted() const;
		void				setPassAccepted();
		bool				isNickSet() const;
		bool				isUserSet() const;
		
		bool				isRegistered() const;
		void				setRegistered(bool val);
		bool				tryRegister();

		void				joinChannel(const std::string& channel);
		void				leaveChannel(const std::string& channel);
		bool				isOnChannel(const std::string& channel) const;
		const std::set<std::string>&	joinedChannels() const;

		void				appendToRecvBuffer(const char* data, std::size_t len);
		bool				extractLine(std::string& out);
		bool				isRecvBufferFull() const;

		void				appendToSendBuffer(const std::string& msg);
		const std::string&	sendBuffer() const;
		void				removeSentData(std::size_t n);
		bool				hasDataToSend() const;

	private:
		int			_fd;
		std::string	_hostname;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		bool		_registered;
		bool		_pass_accepted;
		bool		_nick_set;
		bool		_user_set;
		std::string	_recv_buffer;
		std::string	_send_buffer;
		std::set<std::string>	_joined_channels;

		enum { MAX_RECV_BUFFER = 4096 * 4 };
};

#endif
