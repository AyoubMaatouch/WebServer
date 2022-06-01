#include "socket.hpp"
#include "../config/request.hpp"
#include "helper_tools.hpp"
#include "response.hpp"


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

void Mysocket::start_server(_server &server)
{
	setup_socket(AF_INET, SOCK_STREAM, 0, server);
	bind_socket(server.get_port(), server.get_host().c_str());
	listen_socket();
	accept_connection();
}


void Mysocket::setup_socket(int domain, int type, int protocol, _server &server)
{
	// here you setup sockets for servers
	int on = 1;
	// for each host we need to create a socket
	if ((socketfd = socket(domain, type, protocol)) < 0)
		throw std::runtime_error("setup_socket() failed");
	if ((setsockopt(socketfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on))) < 0)
		throw std::runtime_error("setsockopt() failed");
	server._socketfd = socketfd;
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

void Mysocket::	accept_connection()
{
	//The accept system call grabs the first connection request on the queue of pending connections
	// (set up in listen) and creates a new socket for that connection.
	int rc;
	std::string request;
	
	while (1)
	{
		std::cout << "Waiting on poll()...\n";

		std::cout << "SIZE of POLLFDS : " << pollfds.size() << std::endl;
		struct pollfd *fds = &pollfds[0]; // from vector to array
		rc = poll(fds, pollfds.size(), timeout);
		if (rc < 0)
    	{
		  throw std::runtime_error("poll() failed");
    	  break;
    	}
		
		int addrlen = sizeof(server_addr);		

		for (int i = 0; i < pollfds.size(); i++)
		{
			std::cout << "POLLFDS[" << i << "] : " << pollfds[i].events<< "and revent"<< pollfds[i].revents << std::endl;
			if (pollfds[i].revents & POLLIN)
			{

				if (pollfds[i].fd == socketfd) // check here if POLLIN event is from a new client 
				{
					new_socketfd = accept(socketfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen);
					if (new_socketfd < 0)
						throw std::runtime_error("accept() failed");
					std::cout << "New connections established on: " << new_socketfd<< std::endl << std::endl << std::endl;
					
					// adding new connection here :
					struct pollfd client;
					client.fd = new_socketfd;
					client.events = POLLIN;
					pollfds.push_back(client);
					nfds++;
					// when addig a new connection here we join it with a request object
					std::cout << "----------------\nConnection accepted...\n----------------" << std::endl;
				}
				else // POLLIN event from current client 
				{

					// TO-DO
					// combine here request with socketfd in a map to be able to access it later
					char s[30000];
					// read shoud be protected with fctn for non blocking i/o 
					//and also with to check if the data is chunked or not

					long valread = read(pollfds[i].fd, s, sizeof(s));
					std::string request(s);
					// here check if request is chunked or not
					// here check if request is done the change the pollfds[i].events to POLLOUT
					
					
					
					Request req_obj(request);
					// pollfds.erase(pollfds.begin() + i);
					// nfds--;
				
					// Response res_obj(req_obj);
					// std::string response = res_obj.get_response(); 
					// write(pollfds[i].fd, response.c_str(), response.length());
					// close(pollfds[i].fd);
				}
			}
			else if (pollfds[i].revents & POLLOUT) // POLLOUT event from current client
			{
				// TO-DO here
				// construct response object based on request object
				// then send response object
				// then close socket if the stats is done or change pollfds[i].events to POLLIN

			} else if (pollfds[i].revents & POLLHUP)
			{
				// TO-DO here
				// close socket
				// remove socketfd from pollfds
				// remove socketfd from map
				// remove socketfd from request object
				// remove socketfd from response object
				
				close(pollfds[i].fd);
				pollfds[i].fd = -1;
				pollfds[i].events = 0;
				pollfds.erase(pollfds.begin() + i);
				nfds--;

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
	close(socketfd);
}

