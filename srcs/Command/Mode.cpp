#include "Mode.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#include <sstream>

void Mode::executeAction(Server& server, Client& client, int fd)
{
	if (params().empty())
	{
		server.sendError(client, fd, "461", "MODE :Not enough parameters");
		return;
	}

	Channel* channel = server.findChannel(params()[0]);
	if (channel == NULL)
	{
		server.sendError(client, fd, "403", params()[0] + " :No such channel");
		return;
	}

	if (params().size() == 1)
	{
		sendCurrentModes(server, client, fd, channel);
		return;
	}

	if (!channel->hasMember(&client))
	{
		server.sendError(client, fd, "442", params()[0] + " :You're not on that channel");
		return;
	}
	if (!channel->isOperator(&client))
	{
		server.sendError(client, fd, "482", params()[0] + " :You're not channel operator");
		return;
	}

	std::vector<std::string> modeParams;
	for (size_t i = 2; i < params().size(); i++)
		modeParams.push_back(removeLeadingColon(params()[i]));

	ModeContext ctx(params()[1], modeParams);
	applyModes(server, client, fd, channel, ctx);
	server.setPollout(fd, true);
}

void Mode::sendCurrentModes(Server& server, Client& client, int fd, Channel* channel)
{
	std::string reply = ":ircserv 324 " + client.nickname() + " " + channel->name() + " " + buildModeString(channel) + "\r\n";
	client.appendToSendBuffer(reply);
	server.setPollout(fd, true);
}

std::string Mode::buildModeString(Channel* channel)
{
	std::string modes = "+";
	std::string modeParams;

	if (channel->isInviteOnly())
		modes += "i";
	if (channel->isTopicRestricted())
		modes += "t";
	if (!channel->key().empty())
	{
		modes += "k";
		if (!modeParams.empty())
			modeParams += " ";
		modeParams += channel->key();
	}
	if (channel->userLimit() > 0)
	{
		modes += "l";
		std::ostringstream oss;
		oss << channel->userLimit();
		if (!modeParams.empty())
			modeParams += " ";
		modeParams += oss.str();
	}

	if (!modeParams.empty())
		return modes + " " + modeParams;
	return modes;
}

void Mode::applyModes(Server& server, Client& client, int fd, Channel* channel,
                      ModeContext& ctx)
{
	for (size_t i = 0; i < ctx.modeStr.size(); i++)
	{
		char c = ctx.modeStr[i];

		if (c == '+') { ctx.inputSign = '+'; continue; }
		if (c == '-') { ctx.inputSign = '-'; continue; }

		ModeResult result = dispatchMode(server, client, fd, channel, c, ctx);

		if (result == MODE_STOP)
			break;
		if (result == MODE_SKIP)
			continue;
		appendAppliedMode(c, ctx);
	}

	broadcastModeChange(server, client, channel, ctx.broadcast);
}

Mode::ModeResult Mode::dispatchMode(Server& server, Client& client, int fd, Channel* channel,
                                    char c, ModeContext& ctx)
{
	if (c == 'i')
		return applyModeI(channel, ctx);
	if (c == 't')
		return applyModeT(channel, ctx);
	if (c == 'k')
		return applyModeK(server, client, fd, channel, ctx);
	if (c == 'l')
		return applyModeL(server, client, fd, channel, ctx);
	if (c == 'o')
		return applyModeO(server, client, fd, channel, ctx);
	server.sendError(client, fd, "472", std::string(1, c) + " :is unknown mode char to me");
	return MODE_SKIP;
}

Mode::ModeResult Mode::applyModeI(Channel* channel, ModeContext& ctx)
{
	channel->setInviteOnly(ctx.inputSign == '+');
	return MODE_APPLIED;
}

Mode::ModeResult Mode::applyModeT(Channel* channel, ModeContext& ctx)
{
	channel->setTopicRestricted(ctx.inputSign == '+');
	return MODE_APPLIED;
}

Mode::ModeResult Mode::applyModeK(Server& server, Client& client, int fd, Channel* channel,
                                   ModeContext& ctx)
{
	if (ctx.inputSign == '+')
	{
		if (ctx.paramIndex >= ctx.modeParams.size())
		{
			server.sendError(client, fd, "461", "MODE :Not enough parameters");
			return MODE_STOP;
		}
		channel->setKey(ctx.modeParams[ctx.paramIndex]);
		if (!ctx.broadcast.replyParams.empty())
			ctx.broadcast.replyParams += " ";
		ctx.broadcast.replyParams += ctx.modeParams[ctx.paramIndex];
		ctx.paramIndex++;
	}
	else
	{
		channel->setKey("");
		if (!ctx.broadcast.replyParams.empty())
			ctx.broadcast.replyParams += " ";
		ctx.broadcast.replyParams += "*";
	}
	return MODE_APPLIED;
}

Mode::ModeResult Mode::applyModeL(Server& server, Client& client, int fd, Channel* channel,
                                   ModeContext& ctx)
{
	if (ctx.inputSign == '-')
	{
		channel->setUserLimit(0);
		return MODE_APPLIED;
	}
	if (ctx.paramIndex >= ctx.modeParams.size())
	{
		server.sendError(client, fd, "461", "MODE :Not enough parameters");
		return MODE_STOP;
	}
	std::size_t limit = toSizeT(ctx.modeParams[ctx.paramIndex]);
	ctx.paramIndex++;
	if (limit == 0)
		return MODE_SKIP;
	channel->setUserLimit(limit);
	std::ostringstream oss;
	oss << limit;
	if (!ctx.broadcast.replyParams.empty())
		ctx.broadcast.replyParams += " ";
	ctx.broadcast.replyParams += oss.str();
	return MODE_APPLIED;
}

Mode::ModeResult Mode::applyModeO(Server& server, Client& client, int fd, Channel* channel,
                                   ModeContext& ctx)
{
	if (ctx.paramIndex >= ctx.modeParams.size())
	{
		server.sendError(client, fd, "461", "MODE :Not enough parameters");
		return MODE_STOP;
	}
	Client* target = server.findClientByNick(ctx.modeParams[ctx.paramIndex]);
	if (target == NULL)
	{
		server.sendError(client, fd, "401", ctx.modeParams[ctx.paramIndex] + " :No such nick/channel");
		ctx.paramIndex++;
		return MODE_SKIP;
	}
	if (!channel->hasMember(target))
	{
		server.sendError(client, fd, "441", ctx.modeParams[ctx.paramIndex] + " " + channel->name() + " :They aren't on that channel");
		ctx.paramIndex++;
		return MODE_SKIP;
	}
	channel->setOperator(target, ctx.inputSign == '+');
	if (!ctx.broadcast.replyParams.empty())
		ctx.broadcast.replyParams += " ";
	ctx.broadcast.replyParams += ctx.modeParams[ctx.paramIndex];
	ctx.paramIndex++;
	return MODE_APPLIED;
}

void Mode::appendAppliedMode(char c, ModeContext& ctx)
{
	if (ctx.inputSign != ctx.broadcast.currentSign)
	{
		ctx.broadcast.modeStr += ctx.inputSign;
		ctx.broadcast.currentSign = ctx.inputSign;
	}
	ctx.broadcast.modeStr += c;
}

void Mode::broadcastModeChange(Server& server, Client& client, Channel* channel,
                                const ModeBroadcast& broadcast)
{
	if (broadcast.modeStr.empty())
		return;
	std::string modeMsg = ":" + client.prefix() + " MODE " + channel->name() + " " + broadcast.modeStr;
	if (!broadcast.replyParams.empty())
		modeMsg += " " + broadcast.replyParams;
	modeMsg += "\r\n";
	server.broadcastToChannel(channel, modeMsg);
}

std::string Mode::removeLeadingColon(const std::string& s)
{
	if (!s.empty() && s[0] == ':')
		return s.substr(1);
	return s;
}

std::size_t Mode::toSizeT(const std::string& s)
{
	std::istringstream iss(s);
	std::size_t val = 0;
	iss >> val;
	if (iss.fail())
		return 0;
	return val;
}
