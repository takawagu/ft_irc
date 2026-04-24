#include "Privmsg.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Nick.hpp"

void Privmsg::executeAction(Server& server, Client& client, int fd)
{
	//    Parameters: <receiver>{,<receiver>} <text to be sent>
	if (params().empty())
	{
		server.sendError(client, fd, "461", "PRIVMSG :Not enough parameters");
		return;
	}
	if (params()[0][0] == ':')
	{
		server.sendError(client, fd, "411", "nick :No recipient given (PRIVMSG)");
		return;
	}
	if(params().size() < 2)
	{
		server.sendError(client, fd, "412", "nick :No text to send");
		return;
	}
	if (params()[0][0] == '#')
	{
		return;
	}
	else{
		Client* target = server.findClientByNick(params()[0]);
		if (target == NULL)	
		{
			server.sendError(client, fd, "401", "nick" + params()[0] + " :No such nick/channel");
			return;
		}
		std::string msg = ":" + client.nickname() + " PRIVMSG " + params()[0] + " :" + params()[1] + "\r\n";
		target->appendToSendBuffer(msg);
		server.setPollout(target->fd(), true);
	}
	
}
// ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
//            ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
//            ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
//            ERR_NOSUCHNICK
//            RPL_AWAY
