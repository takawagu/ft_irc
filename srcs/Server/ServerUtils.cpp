#include "Server.hpp"
bool Server::checkPassword(const std::string& pass) const
{ 
    return pass == _config.password(); 
}
