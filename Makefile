# Compiler configuration
CC = g++

# Source files
SRCS = src/main.cpp \
       src/daemon/Daemonizer.cpp \
       src/daemon/LockFile.cpp \
       src/daemon/MattDaemon.cpp \
       src/daemon/SignalHandler.cpp \
       src/network/ClientManager.cpp \
       src/network/Server.cpp \
       src/logger/Tintin_reporter.cpp \
       src/utils/Utils.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable names
NAME = Matt_daemon
BONUS_NAME = Ben_AFK

# Flags
CPPFLAGS = -g -Wall -Wextra -Werror
LDFLAGS = -pthread
INCLUDES = -Iinclude

.PHONY: all bonus clean fclean re

all: $(NAME)

bonus: all $(BONUS_NAME)

# Main executable
$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Pattern rule for object files
%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

# Bonus (Qt CMake project)
$(BONUS_NAME):
	cmake -S ./Bonus \
	      -B ./Bonus/build \
	      -DCMAKE_PREFIX_PATH=/home/vm/Qt/6.11.1/gcc_64 \
	      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$(CURDIR)
	cmake --build ./Bonus/build --config Release

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(BONUS_NAME)
	rm -rf Bonus/build

re: fclean all
