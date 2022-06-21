#include "socket.hpp"

// include make pait library

// #include <boost>
// #include <boost/algorithm/string.hpp>

// #include <bits/stdc++.h>
//! Search later: Network byte order vs. host byte order
// network byte order: big endian (most significant byte first)
// host byte order: little endian (least significant byte first)

// AF_INET = Communication Domain we use IPv4
// SOCK_STREAM socket type is implemented on the TCP (reliable, connection oriented)
// SOCK_DGRAM: UDP(unreliable, connectionless)
// protocol: Protocol value for Internet Protocol(IP), which is 0.
// Biding = Naming the Socket
// The bind function binds the socket to the address specified by the sockaddr structure.
// the structure sockaddr (netinet/in.h) is a generic container that just allows the OS to be able to read the first couple of bytes that identify the address family.
// The sockaddr structure is used to store the address of a socket.
// The htons function takes a 16-bit number in host byte order and returns a 16-bit number in network byte order used in TCP/IP networks
//  htonl converts a long integer (e.g. address) to a network representation
//  htons converts a short integer (e.g. port) to a network representation
// INADDR_ANY is a special value that tells the kernel to assign an address to the socket.
//?https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming



void Mysocket::start_server(std::vector<Server> &servers)
{
	std::map<std::pair<std::string, std::string>, int> binded_servers;

	for (int i = 0; i < servers.size(); i++)
	{
		for (int j = 0; j < servers[i].port.size(); j++)
		{
			if (binded_servers.find(std::make_pair(servers[i].host, servers[i].port[j])) != binded_servers.end())
			{
				int fd = binded_servers[std::make_pair(servers[i].host, servers[i].port[j])];
				server_map[fd].push_back(servers[i]);
				continue;
			}

			int on = 1;
			int _socketfd;
			if ((_socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				throw std::runtime_error("setup_socket() failed");
			if ((setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int))) < 0)
				throw std::runtime_error("setsockopt() failed");
			if (fcntl(_socketfd, F_SETFL, O_NONBLOCK) < 0)
				throw std::runtime_error("fctnl() failed");
			server_addr.sin_family = AF_INET;
			std::cout << "====================================" << std::endl;
			std::cout << "binding host:port 	" << servers[i].host << ":" << servers[i].port[j] << std::endl;
			std::cout << "====================================" << std::endl;
			//==================[adding new sockfd to pollfds]=====================//
			struct pollfd host;
			std::memset(&host, 0, sizeof(host));
			host.fd = _socketfd;
			host.events = POLLIN;
			pollfds.push_back(host);
			nfds = pollfds.size();
			//======================================================================//
			if (servers[i].host == "0.0.0.0")
				server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			else
				server_addr.sin_addr.s_addr = inet_addr(servers[i].host.c_str());
			server_addr.sin_port = htons(atoi(servers[i].port[j].c_str()));
			if (bind(_socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
			{
				throw std::runtime_error("bind_socket() failed");
			}

			if (listen(_socketfd, max_connections) < 0)
				throw std::runtime_error("listen_socket() failed");

			host_socketfd.push_back(_socketfd);
			binded_servers[std::make_pair(servers[i].host, servers[i].port[j])] = _socketfd;
			server_map[_socketfd].push_back(servers[i]);
		}
	}
	binded_servers.clear();
	accept_connection(servers);
}

void Mysocket::accept_connection(std::vector<Server> &servers)
{
	// The accept system call grabs the first connection request on the queue of pending connections
	//  (set up in listen) and creates a new socket for that connection.
	int rc;
	std::string line;
	std::ofstream file;

	//Request req_obj;/* */

	while (1)
	{

		rc = poll(&pollfds.front(), nfds, -1);
		if (rc < 0)
		{
			throw std::runtime_error("poll() failed");
			break;
		}
		int addrlen = sizeof(server_addr);
		for (int i = 0; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents == 0)
			{
				continue;
			}

			if (pollfds[i].revents & POLLHUP || pollfds[i].revents & POLLERR || pollfds[i].revents & POLLNVAL)
			{
				_response_map.erase(pollfds[i].fd);
				_request_map.erase(pollfds[i].fd);
				close(pollfds[i].fd);
				pollfds.erase(pollfds.begin() + i);
				nfds--;
				continue;
			}
			if (pollfds[i].revents & POLLIN)
			{
				if (std::find(host_socketfd.begin(), host_socketfd.end(), pollfds[i].fd) != host_socketfd.end())
				{
					new_socketfd = accept(pollfds[i].fd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen);
					if (new_socketfd < 0)
						throw std::runtime_error("accept() failed");
					std::cout << "New connections established on: " << new_socketfd << std::endl
							  << std::endl
							  << std::endl;
					struct pollfd client;
					std::memset(&client, 0, sizeof(client));
					client.fd = new_socketfd;
					client.events = POLLIN;
					pollfds.push_back(client);
					nfds++;
					_client_map[pollfds[i].fd].push_back(new_socketfd);
					_response_map.insert(std::make_pair(new_socketfd, Response()));
					_request_map.insert(std::make_pair(new_socketfd, Request()));	
				}
				else // POLLIN event from current client
				{
					char s[2048];
					long valread;
					std::map<int, std::vector<Server> >::iterator it = server_map.find(get_hostfd(pollfds[i].fd));
					if (it == server_map.end())
						throw std::runtime_error("No server found for this socket");
					std::vector<Server> servers = it->second;
					
					std::memset(&s, 0, sizeof(s));
					valread = read(pollfds[i].fd, s, sizeof(s) - 1);

					// if error in read change status to pullhub
					if (valread < 0)
						{
							pollfds[i].events = POLLHUP;
							continue;
						}
					s[valread] = '\0';
					
					std::string line(s, valread);
					/************************[Request]*****************************/
					
					std::cout << "=================[B REQUEST]==================" << std::endl;
					std::cout << line << std::endl;
					std::cout << "===================[E REQUEST]=================" << std::endl;
					Server server = get_VaServer(servers, _request_map[pollfds[i].fd].header.host);

					_request_map[pollfds[i].fd].set_request(line);
					Location location = get_location(server, _request_map[pollfds[i].fd].header.path, _request_map[pollfds[i].fd]);
					_request_map[pollfds[i].fd].check_request(server, location);
					if (_request_map[pollfds[i].fd].isFinished())
						pollfds[i].events = POLLOUT;
				}
			}
			if (pollfds[i].revents & POLLOUT) // POLLOUT event from current client
			{
				std::map<int, std::vector<Server> >::iterator it = server_map.find(get_hostfd(pollfds[i].fd));
				if (it == server_map.end())
					throw std::runtime_error("No server found for this socket");
			
				std::vector<Server> servers = it->second;
				Server server = get_VaServer(servers, _request_map[pollfds[i].fd].header.host);
				// std::cout << "Location s" << std::endl;
				Location location = get_location(server, _request_map[pollfds[i].fd].header.path, _request_map[pollfds[i].fd]);
				// std::cout << "Location e" << std::endl;
				_response_map[pollfds[i].fd].set_response(_request_map[pollfds[i].fd], server, location);

				std::string response = _response_map[pollfds[i].fd].get_response(_request_map[pollfds[i].fd], server);
				size_t len = _response_map[pollfds[i].fd].len_send;
	
				std::cout << "Response: " << std::endl << response << std::endl ;
				if (_response_map[pollfds[i].fd].len_send < _response_map[pollfds[i].fd].get_content_length())
				{
					long valwrite ;
					valwrite = write(pollfds[i].fd, response.c_str() + len, (response.length() - len));
					if (valwrite < 0)
						{
							pollfds[i].events = POLLHUP;
							continue;
						}
					_response_map[pollfds[i].fd].len_send += valwrite;
				}
				if (_response_map[pollfds[i].fd].len_send >= _response_map[pollfds[i].fd].get_content_length())
				{
					_response_map[pollfds[i].fd].len_send = 0;
					
					if (_request_map[pollfds[i].fd].header.connection == "keep-alive")
					{
						_request_map[pollfds[i].fd].reload();	
						_response_map[pollfds[i].fd] = Response();
						pollfds[i].events = POLLIN;
					}
					else
					{
						_response_map.erase(pollfds[i].fd);
						_request_map.erase(pollfds[i].fd);
						close(pollfds[i].fd);
						pollfds.erase(pollfds.begin() + i);
						nfds--;
					}
				}
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

Server Mysocket::get_VaServer(std::vector <Server> servers, std::string host)
{
	for (int i = 0; i < servers.size(); i++)
	{
		std::vector<std::string >::iterator it = find(servers[i].server_name.begin(), servers[i].server_name.end(), host);
		if (it != servers[i].server_name.end())
		{
			return servers[i];
		}
	}
	return servers[0];
}

Location Mysocket::get_location(Server server, std::string uri, Request &req)
{
	std::stringstream field(uri);
	std::string item;

	size_t pos = uri.find( "/", 0);
	size_t pos2 = uri.find( "/", pos + 1);
	
	item = "/" + uri.substr(pos+1, pos2 - pos - 1);
	for (int i = 0; i < server.location.size(); i++)
	{
		if (server.location[i].path == item)
		{
			std::cout << req.header.path << std::endl;
			req.header.path = "/" + uri.substr(pos2+1);
			std::cout << " MADe iT IN IF" << std::endl;
			std::cout << req.header.path << std::endl;
			return server.location[i];
		}
	}
	return server.location[0];
} 



int Mysocket::get_hostfd(int fd)
{
	for (std::map<int, std::vector<int> >::iterator it = _client_map.begin(); it != _client_map.end(); it++)
	{
		std::vector<int>::iterator it2 = std::find(it->second.begin(), it->second.end(), fd);
		if (it2 != it->second.end())
		{
			return it->first;
		}
	}
	return -1;
}
