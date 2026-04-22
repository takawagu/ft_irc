#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#include <sstream>
#include <string>
#include <vector>

class Server;
class Client;

class ACommand
{
	public:
		virtual ~ACommand() {}
		void execute(Server& server, Client& client, int fd,const std::string& params);
		virtual void executeAction(Server& server, Client& client, int fd) = 0;

	protected:
		void setParams(const std::string& params);
		const std::vector<std::string>& params() const;

	private:
		std::vector<std::string> _params;
};

#endif
