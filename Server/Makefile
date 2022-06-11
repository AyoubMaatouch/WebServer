# makefile that compile all cpp files in the current directory
# and creates a binary file named "main"

CC =clang++ -g
CFLAGS = -Wall -Wextra -Werror
CFLAGS += -std=c98
SRCS = socket.cpp response.cpp ../config/request.cpp ../config/util.cpp ../config/config_file.cpp helper_tools.cpp main.cpp 
NAME=njinx
all: $(NAME)

$(NAME): $(SRCS)
	$(CC) $(SRCS) -o main

clean:
	rm -f main

run: all
	./main
