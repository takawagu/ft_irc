#include "ACommand.hpp"

void ACommand::execute(Server& server, Client& client, int fd, const std::string& params)
{
    setParams(params);
    executeAction(server, client, fd);
}
void ACommand::setParams(const std::string& params)
{
	_params.clear();
	std::string trimmed = trimEnd(params);
	std::istringstream ss(trimmed);
	std::string token;
	while (ss >> token)
		_params.push_back(token);
}

const std::vector<std::string>& ACommand::params() const
{
	return _params;
}

std::string ACommand::trimEnd(const std::string& str)
{
	std::string trimmed = str;
	while (!trimmed.empty() && (trimmed.back() == '\r' || trimmed.back() == '\n'))
		trimmed.pop_back();
	return trimmed;
}
