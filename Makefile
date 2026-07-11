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
       src/logger/Logger.cpp \
       src/logger/Log.cpp \
       src/utils/Utils.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
NAME = Matt_daemon

# Compilation flags
CPPFLAGS = -g -Wall -Wextra -Werror

# Linker flags
LDFLAGS = -pthread

# Include directories
INCLUDES = -Iinclude

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJS)
	@mkdir -p /var/log/matt_daemon
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all