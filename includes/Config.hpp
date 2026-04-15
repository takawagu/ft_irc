#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>

class Config
{
	public:
		Config(int argc, char** argv);
		Config(const Config&);
		Config& operator=(const Config&);
		~Config();

		int					port() const;
		const std::string&	password() const;

	private:
		enum { MIN_PORT = 1, MAX_PORT = 65535, BASE_10 = 10 };

		int			_port;
		std::string	_password;

		static int	parsePort(const std::string& str);
		static bool	isAllDigits(const std::string& str);
		static void	checkPortRange(long port_num);
};

#endif
