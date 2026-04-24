#include "CommandUtils.hpp"
#include <cctype>

static bool isNicknameSymbol(char c);

bool isChannelNameValid(const std::string& name)
{
	if (name.empty() || name[0] != '#' || name.length() < 2)
		return false;
	for (size_t i = 1; i < name.length(); i++)
	{
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\0')
			return false;
	}
	return true;
}

bool isNicknameValid(const std::string& nick)
{
	if (nick.empty() || nick.length() > MAX_NICKNAME_LENGTH)
		return false;
	if (!isalpha(nick[0]))
		return false;
	for (size_t i = 1; i < nick.length(); i++)
	{
		if (!isalnum(nick[i]) && !isNicknameSymbol(nick[i]))
			return false;
	}
	return true;
}

static bool isNicknameSymbol(char c)
{
	return c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}';
}

std::vector<std::string> splitByComma(const std::string& str)
{
	std::vector<std::string> result;
	std::string current;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (str[i] == ',')
		{
			if (!current.empty())
				result.push_back(current);
			current.clear();
		}
		else
			current += str[i];
	}
	if (!current.empty())
		result.push_back(current);
	return result;
}
