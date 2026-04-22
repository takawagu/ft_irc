#include "Server.hpp"
#include "Client.hpp"

bool Server::checkPassword(const std::string& pass) const
{
    return pass == _config.password();
}

bool Server::isNicknameTaken(const std::string& nick) const
{
    std::map<int, Client*>::const_iterator it = _clients.begin();
    for (; it != _clients.end(); it++)
    {
        if (it->second->nickname() == nick)
            return true;
    }
    return false;
}