#include "Server.hpp"
#include "Client.hpp"

bool Server::checkPassword(const std::string& pass) const
{
    return pass == _config.password();
}

bool Server::isNicknameTaken(const std::string& nick) const
{
    if (_clients.empty())
        return false;

    std::string new_nick=toScandanavianLower(nick);

    for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        const std::string& existing = it->second->nickname();

        // std::cout << "[DEBUG] comparing '" 
        //           << existing << "' with '" 
        //           << nick << "'" << std::endl;

        if (!existing.empty() && toScandanavianLower(existing) == new_nick)
            return true;
    }
    return false;
}

std::string Server::toScandanavianLower(const std::string& nick) const
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
