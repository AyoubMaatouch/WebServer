#include "socket.hpp"
#include "../config/request.hpp"
#include <vector>

#include <map>
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

// function to determine Content-Type based on file extension using a map
std::string	 get_content_type(std::string s)
{
	static std::map<std::string,std::string> mt;

	if (mt.size())
	{
		std::cout << "-====---=-=-=-=[map is not empty]-====---=-=-=-=" << std::endl;
		std::string ext = s.substr(s.find_last_of('.')+1);
		std::map<std::string,std::string>::iterator it = mt.find(ext);
		return it != mt.end() ? it->second : "application/octet-stream";
	}

	mt["html"] = "text/html";
	mt["htm"] = "text/html";
	mt["shtml"] = "text/html";
	mt["css"] = "text/css";
	mt["xml"] = "text/css";
	mt["gif"]= "image/gif";
	mt["jpeg"]= "image/jpeg";
	mt["jpg"]= "image/jpeg";
	mt["js"]= "application/javascript";
	mt["atom"]= "application/atom+xml";
	mt["rss"]= "application/rss+xml";
	
	mt["mml"]= "text/mathml";
	mt["txt"]= "text/plain";
	mt["jad"]= "text/vnd.sun.j2me.app-descriptor";
	mt["wml"]= "text/vnd.wap.wml";
	mt["htc"]= "text/x-component";
	
	mt["png"]= "image/png";
	mt["tif"]= "image/tiff";
	mt["tiff"]= "image/tiff";
	mt["wbmp"]= "image/vnd.wap.wbmp";
	mt["ico"]= "image/x-icon";
	mt["jng"]= "image/x-jng";
	mt["bmp"]= "image/x-ms-bmp";
	mt["svg"]= "image/svg+xml";
	mt["svgz"]= "image/svg+xml";
	mt["webp"]= "image/webp";

	mt["woff"] = "application/font-woff";
	mt["jar"] = "application/java-archive";
	mt["war"] = "application/java-archive";
	mt["ear"] = "application/java-archive";
	mt["json"] = "application/json";
	mt["hqx"] = "application/mac-binhex40";
	mt["doc"] = "application/msword";
	mt["pdf"] = "application/pdf";
	mt["ps"] = "application/postscript";
	mt["eps"] = "application/postscript";
	mt["ai"] = "application/postscript";
	mt["rtf"] = "application/rtf";
	mt["m3u8"] = "application/vnd.apple.mpegurl";
	mt["xls"] = "application/vnd.ms-excel";
	mt["eot"] = "application/vnd.ms-fontobject";
	mt["ppt"] = "application/vnd.ms-powerpoint";
	mt["wmlc"] = "application/vnd.wap.wmlc";
	mt["kml"] = "application/vnd.google-earth.kml+xml";
	mt["kmz"] = "application/vnd.google-earth.kmz";
	mt["7z"] = "application/x-7z-compressed";
	mt["cco"] = "application/x-cocoa";
	mt["jardiff"] = "application/x-java-archive-diff";
	mt["jnlp"] = "application/x-java-jnlp-file";
	mt["run"] = "application/x-makeself";
	mt["pl"] = "application/x-perl";
	mt["pm"] = "application/x-perl";
	mt["prc"] = "application/x-pilot";
	mt["pdb"] = "application/x-pilot";
	mt["rar"] = "application/x-rar-compressed";
	mt["rpm"] = "application/x-redhat-package-manager";
	mt["sea"] = "application/x-sea";
	mt["swf"] = "application/x-shockwave-flash";
	mt["sit"] = "application/x-stuffit";
	mt["tcl"] = "application/x-tcl";
	mt["tk"] = "application/x-tcl";
	mt["der"] = "application/x-x509-ca-cert";
	mt["pem"] = "application/x-x509-ca-cert";
	mt["cert"] = "application/x-x509-ca-cert";
	mt["xpi"] = "application/x-xpinstall";
	mt["xhtml"] = "application/xhtml+xml";
	mt["xspf"] = "application/xspf+xml";
	mt["zip"] = "application/zip";

	mt["bin"] = "application/octet-stream";
	mt["exe"] = "application/octet-stream";
	mt["dll"] = "application/octet-stream";
	mt["deb"] = "application/octet-stream";
	mt["dmg"] = "application/octet-stream";
	mt["iso"] = "application/octet-stream";
	mt["img"] = "application/octet-stream";
	mt["msi"] = "application/octet-stream";
	mt["msp"] = "application/octet-stream";
	mt["msm"] = "application/octet-stream";

	mt["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mt["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mt["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

	mt["mid"] = "audio/midi";
	mt["midi"] = "audio/midi";
	mt["kar"] = "audio/midi";
	mt["mp3"] = "audio/mpeg";
	mt["ogg"] = "audio/ogg";
	mt["m4a"] = "audio/x-m4a";
	mt["ra"] = "audio/x-realaudio";

	mt["3gpp"] = "video/3gpp";
	mt["3gp"] = "video/3gpp";
	mt["ts"] = "video/mp2t";
	mt["mp4"] = "video/mp4";
	mt["mpeg"] = "video/mpeg";
	mt["mpg"] = "video/mpeg";
	mt["mov"] = "video/quicktime";
	mt["webm"] = "video/webm";
	mt["flv"] = "video/x-flv";
	mt["m4v"] = "video/x-m4v";
	mt["mng"] = "video/x-mng";
	mt["asx"] = "video/x-ms-asf";
	mt["asf"] = "video/x-ms-asf";
	mt["wmv"] = "video/x-ms-wmv";
	mt["avi"] = "video/x-msvideo";

	std::string ext = s.substr(s.find_last_of('.') + 1);
	std::map<std::string, std::string>::iterator it = mt.find(ext);
	return it != mt.end() ? it->second : "application/octet-stream";
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
		// acceept should be replaced with select

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
		if (req_obj.header.path == "./")
		{
			s_content_type = get_content_type("index.html") + "\n";
			std::ifstream file1("index.html");
			if (file1.is_open())
			{
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
			
			// [root can be changed from config file if not add "."]

			std::ifstream file1(req_obj.header.path);

			if (file1.is_open())
			{
				std::stringstream s;
				s << file1.rdbuf();
				s_content = s.str();
				int length = s_content.length();
				s_content_length += std::to_string(length);
				file1.close();
			}
		}
		
		std::string response = s_http + s_content_type + s_content_length  + "\n\n" + s_content;
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

