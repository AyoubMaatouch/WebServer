#include "socket.hpp"
#include "../config/request.hpp"
#include "helper_tools.hpp"
#include "response.hpp"
 #include <fcntl.h>
// #include <boost>
// #include <boost/algorithm/string.hpp>

// #include <bits/stdc++.h>
//!Search later: Network byte order vs. host byte order
//network byte order: big endian (most significant byte first)
//host byte order: little endian (least significant byte first)

//AF_INET = Communication Domain we use IPv4
//SOCK_STREAM socket type is implemented on the TCP (reliable, connection oriented)
//SOCK_DGRAM: UDP(unreliable, connectionless)
//protocol: Protocol value for Internet Protocol(IP), which is 0.
//Biding = Naming the Socket
//The bind function binds the socket to the address specified by the sockaddr structure.
//the structure sockaddr (netinet/in.h) is a generic container that just allows the OS to be able to read the first couple of bytes that identify the address family.
//The sockaddr structure is used to store the address of a socket.
//The htons function takes a 16-bit number in host byte order and returns a 16-bit number in network byte order used in TCP/IP networks
// htonl converts a long integer (e.g. address) to a network representation
// htons converts a short integer (e.g. port) to a network representation 
//INADDR_ANY is a special value that tells the kernel to assign an address to the socket.
//?https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming

void Mysocket::start_server(std::vector<Server *> &servers)
{
	for (int i = 0; i < servers.size(); i++)
	{
		for (int j = 0; j < servers[i]->port.size(); j++)
		{
			int on;
			int _socketfd;
			if ((_socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				throw std::runtime_error("setup_socket() failed");
			if ((setsockopt(_socketfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
				throw std::runtime_error("setsockopt() failed");
			if (fcntl(_socketfd, F_SETFL, O_NONBLOCK) < 0)
				throw std::runtime_error("fctnl() failed");

			//=======================================//
			struct pollfd host;
			host.fd = _socketfd;
			host.events = POLLIN;
			pollfds.push_back(host);
			nfds = pollfds.size();
			//=======================================//
			server_addr.sin_family = AF_INET;
			inet_pton(AF_INET, servers[i]->host.c_str(), &(server_addr.sin_addr.s_addr));
			server_addr.sin_port = htons(atoi(servers[i]->port[j].c_str()));

			if (bind(_socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
				throw std::runtime_error("bind_socket() failed");

			if (listen(_socketfd, max_connections) < 0)
				throw std::runtime_error("listen_socket() failed");
			host_socketfd.push_back(_socketfd);
			// setup_socket(AF_INET, SOCK_STREAM, 0);
			// std::cout << "binding to host:port: " << servers[i]->host <<":"<< servers[i]->port[j] << std::endl;
			// bind_socket(atoi(servers[i]->port[j].c_str()), servers[i]->host.c_str());
			// listen_socket();
		}
	}
	// setup_socket(AF_INET, SOCK_STREAM, 0);
	// bind_socket(servers.get_port(), server.get_host().c_str());
	// listen_socket();
	accept_connection(servers);
}


void Mysocket::setup_socket(int domain, int type, int protocol)
{
	// here you setup sockets for servers
	// for each host we need to create a socket
	int on;
	if ((socketfd = socket(domain, type, protocol)) < 0)
		throw std::runtime_error("setup_socket() failed");
	if ((setsockopt(socketfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
		throw std::runtime_error("setsockopt() failed");
	if (fcntl(socketfd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("fctnl() failed");
	// server._socketfd = socketfd;
	struct pollfd host;
	host.fd = socketfd;
	host.events = POLLIN;
	pollfds.push_back(host);
	nfds = pollfds.size();
	
}

void Mysocket::bind_socket(int port, const char* ip)
{
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &(server_addr.sin_addr.s_addr));
	server_addr.sin_port = htons(port);

	if (bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		throw std::runtime_error("bind_socket() failed");
}

void Mysocket::listen_socket()
{
	//Listen second parameter = backlog = defines the maximum number of pending connections that can be queued up before connections are refused.
	// liste on all connections using pollfds.fd
	if (listen(socketfd, max_connections) < 0)
		throw std::runtime_error("listen_socket() failed");
}

void Mysocket::accept_connection(std::vector<Server *> &servers)
{
	//The accept system call grabs the first connection request on the queue of pending connections
	// (set up in listen) and creates a new socket for that connection.
	int rc;
	std::string request;
	std::ofstream file;

	Request req_obj;
	
	while (1)
	{
		std::cout << "Waiting on poll()...\n";
		std::cout <<"SIZE of POLLFDS : " << pollfds.size() << std::endl;
		// struct pollfd *fds = &pollfds[0]; // from vector to array
		rc = poll(&pollfds.front(), nfds, -1);
		if (rc < 0)
    	{
		  throw std::runtime_error("poll() failed");
    	  break;
    	}
		int addrlen = sizeof(server_addr);		
		for (int i = 0; i < pollfds.size(); i++)
		{
			std::cout << "poll() returned " << rc << std::endl;
			if (pollfds[i].revents == 0)
			{
				std::cout << "No events\n";
				continue;
			}
			std::cout << "events: " << pollfds[i].events << " revents :" << pollfds[i].revents << std::endl;
			if (pollfds[i].revents & POLLHUP || pollfds[i].revents & POLLERR ||   pollfds[i].revents & POLLNVAL)
			{
				printf("Client disconnected\n");
				close(pollfds[i].fd);
				pollfds.erase(pollfds.begin() + i);
				nfds--;
				continue;
			}
			if (pollfds[i].revents & POLLIN)
			{
				if (std::find(host_socketfd.begin(), host_socketfd.end(), pollfds[i].fd) != host_socketfd.end())
				{
					new_socketfd = accept(pollfds[i].fd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen);
					if (new_socketfd < 0)
						throw std::runtime_error("accept() failed");
					std::cout << "New connections established on: " << new_socketfd<< std::endl << std::endl << std::endl;
					// adding new connection here :
					struct pollfd client;
					client.fd = new_socketfd;
					client.events = POLLIN;
					pollfds.push_back(client);
					nfds++;
					std::cout << "----------------\nConnection accepted...\n----------------" << std::endl;
					break ;
				}
				else // POLLIN event from current client 
				{

					// TO-DO
					// combine here request with socketfd in a map to be able to access it later
					char s[1024];
					// read shoud be protected with fctn for non blocking i/o 
					//and also with to check if the data is chunked or not

					std::cout << "----------------\nData received...\n----------------" << std::endl;
					long valread;

					std::memset(&s, 0, sizeof(s));
					// std::cout << "Waiting on read()...\n";
					valread = read(pollfds[i].fd, s, sizeof(s)) ;
					std::cout << "valread : " << valread << std::endl;
					s[valread] = '\0';
					std::string request(s);
					// std::cout << "request : " << std::endl << request ;
					file.open("log.txt", std::ios::app);

					file << request;
					file.close();
					// std::cout << "request : " << std::endl << request;
					req_obj.set_request(request);
					req_obj.check_request(servers[i]);
					exit(0);
					if (req_obj.isFinished())
					{
						std::cout << "----------------\nRequest finished...\n----------------" << std::endl;
						// file.close();
						exit(0);
						
					}
				}
			}
			if (pollfds[i].revents & POLLOUT) // POLLOUT event from current client
			{
				// TO-DO here
				// construct response object based on request object
				// then send response object
				// then close socket if the stats is done or change pollfds[i].events to POLLIN

			}  
		}
		}
		
		}


Mysocket::Mysocket()
{
	
	// https://stackoverflow.com/questions/10002868/what-value-of-backlog-should-i-use
	timeout = (3 * 60 * 1000); // 3 min
	this->max_connections = 128;

}


Mysocket::~Mysocket()
{
	for (int i = 0; i < pollfds.size(); i++)
	{
		close(pollfds[i].fd);
	}
}

