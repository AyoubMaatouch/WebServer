# makefile that compile all cpp files in the current directory
# and creates a binary file named "main"

CC =clang++ 
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -std=c98

SRCS = socket.cpp response.cpp cgi.cpp helper_tools.cpp main.cpp
CONFIG = ../configuration/request.cpp ../configuration/util.cpp ../configuration/config_file.cpp  

NAME=njinx

all: $(NAME)

# $(NAME) : $(SRCS) $(CONFIG)
# 	$(CC) $(CFLAGS) -o $(NAME) $(SRCS) $(CONFIG)
$(NAME): $(SRCS)
	$(CC) $(SRCS) $(CONFIG) -o njinx
clean:
	rm -f njinx

run: all
	./njinx
