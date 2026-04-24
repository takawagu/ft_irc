#include "ACommand.hpp"

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
