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
#include "../config/config_file.hpp"
#include "../config/library.hpp"


class Mysocket
{
	public:

		Mysocket();
		void start_server(std::vector<Server *> &servers);
		~Mysocket();

		void setup_socket(int domain, int type, int protocol);
		void bind_socket(int port, const char* ip);
		void listen_socket();
		void accept_connection(std::vector<Server *> &servers);

		
	private:
	
	int timeout;
	int nfds;
	std::vector<struct pollfd> pollfds;

	int						socketfd;
	std::vector<int>		host_socketfd;
	struct sockaddr_in		server_addr;
	int						new_socketfd;
	int						max_connections;
};
