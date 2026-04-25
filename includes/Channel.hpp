#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <string>
#include <cstddef>

class Client;

class Channel
{
	public:
		struct Modes
		{
			bool        invite_only;
			bool        topic_restricted;
			std::string key;
			std::size_t user_limit;

			Modes() : invite_only(false), topic_restricted(false), key(), user_limit(0) {}
		};

		struct Topic
		{
			std::string text;
			std::string setter;
		};

		explicit Channel(const std::string& name);
		Channel(const Channel&);
		Channel& operator=(const Channel&);
		~Channel();

		const std::string&	name() const;
		const std::string&	topic() const;
		const std::string&	topicSetter() const;
		void				setTopic(const std::string& topic, const std::string& setter);

		void				addMember(Client* client, bool isOperator = false);
		void				removeMember(Client* client);
		bool				hasMember(Client* client) const;
		bool				isOperator(Client* client) const;
		void				setOperator(Client* client, bool val);
		std::size_t			memberCount() const;
		const std::map<Client*, bool>&	members() const;

		bool				isInviteOnly() const;
		void				setInviteOnly(bool val);
		bool				isInvited(Client* client) const;
		void				addInvite(Client* client);
		void				removeInvite(Client* client);

		bool				isTopicRestricted() const;
		void				setTopicRestricted(bool val);

		const std::string&	key() const;
		void				setKey(const std::string& k);

		std::size_t			userLimit() const;
		void				setUserLimit(std::size_t limit);

		void				broadcast(const std::string& msg, Client* exclude = NULL);

	private:
		std::string				_name;
		Topic					_topic;
		std::map<Client*, bool>	_members;
		std::set<Client*>	_invited;
		Modes					_modes;
};

#endif
