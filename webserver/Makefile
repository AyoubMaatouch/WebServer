# makefile that compile all cpp files in the current directory
# and creates a binary file named "main"

CC =clang++ -g -fsanitize=address
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -std=c++98

SRCS = socket.cpp response.cpp cgi.cpp helper_tools.cpp main.cpp
CONFIG = ../configuration/request.cpp ../configuration/util.cpp ../configuration/config_file.cpp

all: webserv

webserv: $(SRCS) $(CONFIG)
	$(CC) $(CFLAGS) $(SRCS) $(CONFIG) -o webserv 
clean:
	rm -rf webserv.o
fclean : clean
		rm -rf ./webserv webserv.dSYM
re : fclean all