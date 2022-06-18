# makefile that compile all cpp files in the current directory
# and creates a binary file named "main"

CC =clang++  -g -fsanitize=address
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -std=c98

SRCS = socket.cpp response.cpp cgi.cpp helper_tools.cpp main.cpp
CONFIG = ../configuration/request.cpp ../configuration/util.cpp ../configuration/config_file.cpp  

NAME=njinx

all: $(SRCS)
	$(CC)  $(SRCS) $(CONFIG) -o $(NAME)
	./$(NAME)

clean:
	rm -f njinx
