#include "ACommand.hpp"

void ACommand::execute(Server& server, Client& client, int fd, const std::vector<std::string>& params)
{
	setParams(params);
	executeAction(server, client, fd);
}

void ACommand::setParams(const std::vector<std::string>& params)
{
	_params = params;
}

const std::vector<std::string>& ACommand::params() const
{
	return _params;
}
