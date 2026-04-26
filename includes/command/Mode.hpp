#ifndef MODE_HPP
#define MODE_HPP

#include "ACommand.hpp"
#include <string>
#include <vector>
#include <cstddef>

class Channel;

class Mode : public ACommand
{
	public:
		void executeAction(Server& server, Client& client, int fd);

	private:
		enum ModeResult { MODE_APPLIED, MODE_SKIP, MODE_STOP };

		struct ModeBroadcast
		{
			ModeBroadcast() : currentSign('\0') {}
			std::string modeStr;
			std::string replyParams;
			char        currentSign;
		};

		struct ModeContext
		{
			ModeContext(const std::string& ms, const std::vector<std::string>& mp)
				: modeStr(ms), modeParams(mp), paramIndex(0), inputSign('+') {}
			const std::string&              modeStr;
			const std::vector<std::string>& modeParams;
			std::size_t   paramIndex;
			char          inputSign;
			ModeBroadcast broadcast;
		};

		static void        sendCurrentModes(Server& server, Client& client, int fd, Channel* channel);
		static std::string buildModeString(Channel* channel);
		static void        applyModes(Server& server, Client& client, int fd, Channel* channel,
		                              ModeContext& ctx);
		static ModeResult  dispatchMode(Server& server, Client& client, int fd, Channel* channel,
		                                char c, ModeContext& ctx);
		static ModeResult  applyModeI(Channel* channel, ModeContext& ctx);
		static ModeResult  applyModeT(Channel* channel, ModeContext& ctx);
		static ModeResult  applyModeK(Server& server, Client& client, int fd, Channel* channel,
		                              ModeContext& ctx);
		static ModeResult  applyModeL(Server& server, Client& client, int fd, Channel* channel,
		                              ModeContext& ctx);
		static ModeResult  applyModeO(Server& server, Client& client, int fd, Channel* channel,
		                              ModeContext& ctx);
		static void        appendAppliedMode(char c, ModeContext& ctx);
		static void        broadcastModeChange(Server& server, Client& client, Channel* channel,
		                                       const ModeBroadcast& broadcast);
		static std::string removeLeadingColon(const std::string& s);
		static std::size_t toSizeT(const std::string& s);
};

#endif
