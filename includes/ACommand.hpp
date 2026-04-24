#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <string>
#include <vector>

class Server;
class Client;

class ACommand
{
	public:
		virtual ~ACommand() {}
		void execute(Server& server, Client& client, int fd, const std::vector<std::string>& params);
		virtual void executeAction(Server& server, Client& client, int fd) = 0;

	protected:
		const std::vector<std::string>& params() const;

	private:
		void setParams(const std::vector<std::string>& params);
		std::vector<std::string> _params;
};

// utils
std::vector<std::string> splitByComma(const std::string& str);

#endif
