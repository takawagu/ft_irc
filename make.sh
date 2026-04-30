#!/bin/bash

# Makefileを生成するシェルスクリプト

# ファイル一覧を取得
echo "Generating file list from src directory..."
srcs=$(find srcs -name "*.cpp" | sed 's|^src/||; s|$| \\|')
INC_DIRS=$(find includes -type d | sort | sed 's|$| \\|')

# # ソースファイル一覧
# SRCS=$(find srcs -name "*.cpp" | sort)

# # includeディレクトリ一覧
# INC_DIRS=$(find includes -type d | sort)

# Makefileを生成
cat > Makefile << 'EOF'
NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic

INC_DIR = includes
SRCS_DIR = srcs
OBJ_DIR = obj

INC_DIRS = \
EOF
echo "$INC_DIRS" >> Makefile

cat >> Makefile << 'EOF'

INCLUDES = $(addprefix -I,$(INC_DIRS))

SRCS := \
EOF

# ファイル一覧を追加
echo "$srcs" >> Makefile

# Makefileの残りの部分を追加
cat >> Makefile << 'EOF'

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

EOF

echo "Makefile has been generated successfully!"
