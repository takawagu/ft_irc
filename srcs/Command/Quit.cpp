#include "Quit.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

void Quit::executeAction(Server& server, Client& client, int fd)
{
	std::string reason = "";
	if (!params().empty())
		reason = params()[0];

	std::string quitMsg = ":" + client.prefix() + " QUIT :" + reason + "\r\n";

	const std::set<std::string> channels = client.joinedChannels();
	for (std::set<std::string>::const_iterator it = channels.begin(); it != channels.end(); ++it)
	{
		Channel* channel = server.findChannel(*it);
		if (channel)
			channel->broadcast(quitMsg, &client);
	}

	server.addToDisconnectList(fd);
}
