#ifndef COMMANDUTILS_HPP
#define COMMANDUTILS_HPP

#include <string>
#include <vector>

#define MAX_NICKNAME_LENGTH 9

bool isChannelNameValid(const std::string& name);
bool isNicknameValid(const std::string& nick);
std::vector<std::string> splitByComma(const std::string& str);

#endif
