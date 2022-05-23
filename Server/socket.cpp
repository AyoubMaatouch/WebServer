#include "socket.hpp"
#include "../config/request.hpp"
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

// function to determine Content-Type based on file extension
std::string get_content_type(std::string file_name)
{
	std::string content_type = "text/plain";
	if (file_name.find(".html") != std::string::npos)
		content_type = "text/html";
	else if (file_name.find(".css") != std::string::npos)
		content_type = "text/css";
	else if (file_name.find(".js") != std::string::npos)
		content_type = "application/javascript";
	else if (file_name.find(".png") != std::string::npos)
		content_type = "image/png";
	else if (file_name.find(".jpg") != std::string::npos)
		content_type = "image/jpeg";
	else if (file_name.find(".jpeg") != std::string::npos)
		content_type = "image/jpeg";
	else if (file_name.find(".gif") != std::string::npos)
		content_type = "image/gif";
	else if (file_name.find(".svg") != std::string::npos)
		content_type = "image/svg+xml";
	else if (file_name.find(".ico") != std::string::npos)
		content_type = "image/x-icon";
	else if (file_name.find(".txt") != std::string::npos)
		content_type = "text/plain";
	else if (file_name.find(".xml") != std::string::npos)
		content_type = "text/xml";
	else if (file_name.find(".pdf") != std::string::npos)
		content_type = "application/pdf";
	else if (file_name.find(".zip") != std::string::npos)
		content_type = "application/zip";
	else if (file_name.find(".gz") != std::string::npos)
		content_type = "application/gzip";
	else if (file_name.find(".tar") != std::string::npos)
		content_type = "application/x-tar";
	else if (file_name.find(".swf") != std::string::npos)
		content_type = "application/x-shockwave-flash";
	else if (file_name.find(".mp3") != std::string::npos)
		content_type = "audio/mpeg";
	else if (file_name.find(".wav") != std::string::npos)
		content_type = "audio/x-wav";
	else if (file_name.find(".wma") != std::string::npos)
		content_type = "audio/x-ms-wma";
	else if (file_name.find(".mp4") != std::string::npos)
		content_type = "video/mp4";
	else if (file_name.find(".mpeg") != std::string::npos)
		content_type = "video/mpeg";
	else if (file_name.find(".mpg") != std::string::npos)
		content_type = "video/mpeg";
	else if (file_name.find(".avi") != std::string::npos)
		content_type = "video/x-msvideo";
	else if (file_name.find(".mov") != std::string::npos)
		content_type = "video/quicktime";
	else if (file_name.find(".flv") != std::string::npos)
		content_type = "video/x-flv";
	else if (file_name.find(".mp2") != std::string::npos)
		content_type = "audio/mpeg";
	else if (file_name.find(".m4a") != std::string::npos)
		content_type = "audio/mp4";
	else if (file_name.find(".m4v") != std::string::npos)
		content_type = "video/mp4";
	else if (file_name.find(".m3u8") != std::string::npos)
		content_type = "application/x-mpegURL";
	else if (file_name.find(".m3u") != std::string::npos)
		content_type = "audio/x-mpegurl";
	else if (file_name.find(".ts") != std::string::npos)
		content_type = "video/MP2T";
	else if (file_name.find(".3gp") != std::string::npos)
		content_type = "video/3gpp";
	else if (file_name.find(".3g2") != std::string::npos)
		content_type = "video/3gpp2";
	else if (file_name.find(".m2ts") != std::string::npos)
		content_type = "video/MP2T";
	else if (file_name.find(".mkv") != std::string::npos)
		content_type = "video/x-matroska";
	else if (file_name.find(".mka") != std::string::npos)
		content_type = "audio/x-matroska";
	else if (file_name.find(".webm") != std::string::npos)
		content_type = "video/webm";
	else if (file_name.find(".ogv") != std::string::npos)
		content_type = "video/ogg";
	else if (file_name.find(".oga") != std::string::npos)
		content_type = "audio/ogg";
	else if (file_name.find(".ogx") != std::string::npos)
		content_type = "application/ogg";
	else if (file_name.find(".opus") != std::string::npos)
		content_type = "audio/ogg";
	else if (file_name.find(".spx") != std::string::npos)
		content_type = "audio/ogg";
	else if (file_name.find(".aac") != std::string::npos)
		content_type = "audio/aac";
	else if (file_name.find(".flac") != std::string::npos)
		content_type = "audio/flac";
	else if (file_name.find(".dts") != std::string::npos)
		content_type = "audio/vnd.dts";
	else if (file_name.find(".dtshd") != std::string::npos)
		content_type = "audio/vnd.dts.hd";
	return content_type;
}

void Mysocket::setup_socket(int domain, int type, int protocol)
{
	if ((socketfd = socket(domain, type, protocol)) < 0)
		throw std::runtime_error("setup_socket() failed");
}

void Mysocket::bind_socket(int port)
{
	server_addr.sin_family = AF_INET;
	// only accept here the host example " = inet_addr("127.0.0.1") ;"
	inet_pton(AF_INET, "127.0.0.1", &(server_addr.sin_addr.s_addr));
	//  server_addr.sin_addr.s_addr = INADDR_ANY;
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
		char s[30000];
		long valread = read(new_socketfd, s, 30000);
		std::string str(s);
		std::cout << "Request: " << std::endl << str << std::endl;
		Request req_obj(str);
		
		std::cout  << "||----" <<  req_obj.header.path  <<  "||----"  << std::endl;
		//https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
	
		std::string s_http = "HTTP/1.1 200 OK\n";
		std::string s_content_type ;
		std::string s_content_length = "Content-Length: ";
		std::string s_content;
		int content_length = 0;
		if (req_obj.header.path == "/")
		{
			s_content_type = get_content_type("index.html") + "\n";
			std::ifstream file1("index.html");
			if (file1.is_open())
			{
				std::cout << "are you here?" << std::endl;
				std::stringstream s;
				s << file1.rdbuf();
				s_content = s.str();
				int length = s_content.length();
				s_content_length += std::to_string(length);
				file1.close();
			}
		}
		else
		{
			s_content_type = get_content_type(req_obj.header.path) + "\n";
			std::cout << "[s_content_type]: " << s_content_type << std::endl;

			std::ifstream file1(req_obj.header.path);

			if (file1.is_open())
			{
				std::cout << "is image open" << std::endl;
				std::stringstream s;
				s << file1.rdbuf();
				s_content = s.str();
				int length = s_content.length();
				s_content_length += std::to_string(length);
				file1.close();
			}
		}
		
		std::string response = s_http + s_content_type + s_content_length  + "\n\n" + s_content;
		std::cout << "-------------------Response: ---------------------" << std::endl << response << std::endl;
		std::cout << "-------------------[END Response]---------------------" << std::endl;
		write(new_socketfd, response.c_str(), response.length());
		close(new_socketfd);

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

