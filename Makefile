NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

INC_DIR = includes
SRCS_DIR = srcs
OBJ_DIR = obj

INC_DIRS = \
includes \
includes/command \

INCLUDES = $(addprefix -I,$(INC_DIRS))

SRCS := \
srcs/Channel/Channel.cpp \
srcs/Channel/ChannelMember.cpp \
srcs/Channel/ChannelMode.cpp \
srcs/Client.cpp \
srcs/Command/ACommand.cpp \
srcs/Command/Cap.cpp \
srcs/Command/CommandUtils.cpp \
srcs/Command/Invite.cpp \
srcs/Command/Join.cpp \
srcs/Command/Kick.cpp \
srcs/Command/Mode.cpp \
srcs/Command/Nick.cpp \
srcs/Command/Part.cpp \
srcs/Command/Pass.cpp \
srcs/Command/Privmsg.cpp \
srcs/Command/Quit.cpp \
srcs/Command/Topic.cpp \
srcs/Command/User.cpp \
srcs/Config.cpp \
srcs/Server/Server.cpp \
srcs/Server/ServerAccept.cpp \
srcs/Server/ServerChannel.cpp \
srcs/Server/ServerClient.cpp \
srcs/Server/ServerCommand.cpp \
srcs/Server/ServerIn.cpp \
srcs/Server/ServerLoop.cpp \
srcs/Server/ServerOut.cpp \
srcs/Server/ServerSetup.cpp \
srcs/Server/ServerShutdown.cpp \
srcs/Server/ServerUtils.cpp \
srcs/commandTest.cpp \
srcs/main.cpp \

OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(OBJ_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
