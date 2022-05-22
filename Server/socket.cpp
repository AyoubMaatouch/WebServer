#include "socket.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>

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


void Mysocket::setup_socket(int domain, int type, int protocol)
{
	if ((socketfd = socket(domain, type, protocol)) < 0)
		throw std::runtime_error("setup_socket() failed");
}

void Mysocket::bind_socket(int port)
{
	server_addr.sin_family = AF_INET;
	// only accept here the host example " = inet_addr("127.0.0.1") ;"
	//inet_pton(AF_INET, "10.1.32.241", &(server_addr.sin_addr.s_addr));
	 server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		throw std::runtime_error("bind_socket() failed");
}

void Mysocket::listen_socket()
{
		//Listen second parameter = backlog = defines the maximum number of pending connections that can be queued up before connections are refused.
	if (listen(socketfd, max_connections) < 0)
		throw std::runtime_error("listen_socket() failed");
}

int Mysocket::get_socketfd()
{
	return (socketfd);
}

int Mysocket::get_new_socketfd()
{
	return (new_socketfd);
}
//function to split a string into a vector of strings
std::vector<std::string> split(const std::string &s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

void Mysocket::	accept_connection()
{
	//The accept system call grabs the first connection request on the queue of pending connections
	// (set up in listen) and creates a new socket for that connection.
	while (1)
	{
		std::cout << "----------------\nWaiting for connection...\n----------------" << std::endl;
		int addrlen = sizeof(server_addr);
		if ((new_socketfd = accept(socketfd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0)
		{
			std::cout << strerror(errno) << std::endl;
			throw std::runtime_error("accept_connection() failed");
		}
		else
			std::cout << "Connected" << std::endl;

		//char *response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 261\n\n<!DOCTYPE html><html><head><title>Our Company</title></head><body><h1>Welcome to Our Company</h1><h2>Web Site Main Ingredients:</h2><p>Pages (HTML)</p><p>Style Sheets (CSS)</p><p>Computer Code (JavaScript)</p><p>Live Data (Files and Databases)</p></body></html>";
		//https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
	
		std::string s_http = "HTTP/1.1 200 OK\n";
		// std::string s_content_type = "Content-Type: text/html\n";
		std::string s_content_length = "Content-Length: ";
		std::string s_content;

		
		char s[30000];
		long valread = read(new_socketfd, s, 30000);
		std::string s_request(s);
		// read the first line of the request
		std::string s_first_line = s_request.substr(0, s_request.find("\r\n"));
		std::cout << "First LINE here ::   "<<s_first_line << std::endl;
		//spilt the first line
		// include split function
	


		std::vector<std::string> v_first_line = split(s_first_line, ' ');
		std::string s_method = v_first_line[0];
		std::string s_path = v_first_line[1];
		std::string s_http_version = v_first_line[2];
		std::cout << "Method ::   " << s_method << std::endl;
		std::cout << "Path ::   " << s_path << std::endl;
		std::cout << "HTTP version ::   " << s_http_version << std::endl;


		if (s_path == "/paris.png")
		{
			std::cout << "ARE YOU HERE ????" << std::endl;
			std::string s_content_type = "Content-Type: image/png\n";
			std::string s_content_length = "Content-Length: ";
			std::string s_content;

			std::ifstream file("paris.png", std::ios::binary);
			if (file.is_open())
			{
				file.seekg(0, std::ios::end);
				int length = file.tellg();
				file.seekg(0, std::ios::beg);
				s_content_length += std::to_string(length);
				std::stringstream s;
				s << file.rdbuf();
				s_content = s.str();
				file.close();
			}
			int content_length = s_content.length();
			std::string response = s_http  + s_content_length + std::to_string(content_length) + "\n\n" + s_content;
			write(new_socketfd, response.c_str(), response.length());
			close(new_socketfd);
		}
		else
		{
			std::ifstream file("index.html");
			std::string tmp;
			while (getline (file, tmp))
				s_content += tmp;

			int content_length = s_content.length();


			std::string response = s_http  + s_content_length + std::to_string(content_length) + "\n\n" + s_content;

			std::cout << "Message from client: " << s << std::endl;
			write(new_socketfd, response.c_str(), response.length());
			close(new_socketfd);
		}
	}
}


Mysocket::Mysocket(int domain, int type, int protocol, int port, int max_connections)
{
	//! set errno
	this->max_connections = max_connections;
	setup_socket(domain, type, protocol);
	bind_socket(port);
	listen_socket();
	accept_connection();

}

Mysocket::~Mysocket()
{
	close(socketfd);
}
