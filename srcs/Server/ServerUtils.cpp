#include "Server.hpp"
#include "Client.hpp"

bool Server::checkPassword(const std::string& pass) const
{
    return pass == _config.password();
}

std::string toScandanavianLower(const std::string& nick);

bool Server::isNicknameTaken(const std::string& nick) const
{
    std::map<int, Client*>::const_iterator it = _clients.begin();
    for (; it != _clients.end(); it++)
    {
        std::string client_nick = toScandanavianLower(it->second->nickname());
        std::string new_nick=toScandanavianLower(nick);
        if (client_nick == new_nick)
            return true;
    }
    return false;
}

std::string toScandanavianLower(const std::string& nick)
{
    std::string new_nick=nick;
    for (size_t i = 0; i < new_nick.length(); i++)
    {
        if (new_nick[i] == '{')
            new_nick[i] = '[';
        else if (new_nick[i] == '}')
            new_nick[i] = ']';
        new_nick[i] = tolower(new_nick[i]);
    }
    return new_nick;
}