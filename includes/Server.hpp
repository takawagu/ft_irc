#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>

#include "Config.hpp"

class Server
{
	public:
		explicit Server(const Config& config);
		Server(const Server&);
		Server& operator=(const Server&);
		~Server();

		void run();

	private:
		const Config&	_config;
};

#endif
