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

    while (ss >> token) {
        if (!token.empty() && token[0] == ':') {
            // ':' を除いて残り全部を1つのパラメータにする
            std::string rest = token.substr(1);
            std::string tail;
            std::getline(ss, tail);
            if (!tail.empty())
                rest += tail;
            _params.push_back(rest);
            break;
        }
        _params.push_back(token);
    }

    // デバッグ出力
    for (size_t i = 0; i < _params.size(); ++i)
        std::cout << "params[" << i << "]=\"" << _params[i] << "\"" << std::endl;
}

// void ACommand::setParams(const std::string& params)
// {
// 	_params.clear();
// 	std::istringstream ss(params);
// 	std::string token;
// 	while (ss >> token){
// 		_params.push_back(token);
// 		if (ss[0] == ':')
// 		{
// 			_params.push_back(ss);
// 			break;
// 		}
		
// 	}

// 	//デバッグ出力用テスト
// 	for (size_t i = 0; i < _params.size(); ++i)
// 		std::cout << "params[" << i << "]=\"" << _params[i] << "\"" << std::endl;
// }

const std::vector<std::string>& ACommand::params() const
{
	return _params;
}
