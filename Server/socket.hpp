#pragma once

#include <poll.h>
#include <vector>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "Server.hpp"
#include "../config_file.hpp"


class Mysocket
{
	public:

		Mysocket();
		void start_server(_server &server);
		~Mysocket();

		void setup_socket(int domain, int type, int protocol, _server &server);
		void bind_socket(int port, const char* ip);
		void listen_socket();
		void accept_connection();

		
	private:
	
	int timeout;
	int nfds;
	std::vector<struct pollfd> pollfds;
	int					socketfd;
	struct sockaddr_in	server_addr;
	int					new_socketfd;
	int					max_connections;
};
