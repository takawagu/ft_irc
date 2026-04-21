#include "ACommand.hpp"
#include <iostream>

void ACommand::execute(Server& server, Client& client, int fd, const std::string& params)
{
    setParams(params);
    executeAction(server, client, fd);
}

void ACommand::setParams(const std::string& params)
{
	_params.clear();
	std::istringstream ss(params);
	std::string token;
	while (ss >> token)
		_params.push_back(token);

	//デバッグ出力用テスト
	for (size_t i = 0; i < _params.size(); ++i)
		std::cout << "params[" << i << "]=\"" << _params[i] << "\"" << std::endl;
}

const std::vector<std::string>& ACommand::params() const
{
	return _params;
}
