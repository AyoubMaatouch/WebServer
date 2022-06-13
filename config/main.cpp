#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read

#include "request.hpp"

int main()
{
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen to port 9990 on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(9937); // htons is necessary to convert a number to
									 // network byte order
	if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cout << "Failed to bind to port 9985. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Start listening. Hold at most 10 connections in the queue
	if (listen(sockfd, 10) < 0)
	{
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Grab a connection from the queue
	auto addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr *)&sockaddr, (socklen_t *)&addrlen);
	if (connection < 0)
	{
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Read from the connection
	char buffer[30000];
	// auto bytesRead;
	Request req;
	while (!req.isFinished())
	{
		read(connection, buffer, 30000);
		req.set_request(buffer);
	}
	// req.test_output();

	// Send a message to the connection
	std::string response = "Good talking to you\n";
	send(connection, response.c_str(), response.size(), 0);

	// Close the connections
	close(connection);
	close(sockfd);
}

// #include "library.hpp"

// void test_list(std::vector<std::string> list)
// {
// 	for (size_t i = 0; i < list.size(); i++)
// 	{
// 		std::cout << list[i] << "-";
// 	}
// }

// void locations(std::vector<Location> locs)
// {
// 	for (size_t i = 0; i < locs.size(); i++)
// 	{
// 		std::cout << "\n** LOCATION " << i << " **\n\n";
// 		std::cout << "\t" << "path : [" << locs[i].path << "]\n";
// 		std::cout << "\t" << "root : [" << locs[i].root << "]\n";
// 		std::cout << "\t" << "upload : [" << locs[i].upload << "]\n";
// 		std::cout << "\t" << "cgi : [" << locs[i].cgi << "]\n";
// 		std::cout << "\t" << "auto_index : [" << (locs[i].auto_index ? "on" : "off") << "]\n";
// 		std::cout << "\t" << "index : ["; test_list(locs[i].index); std::cout << "]\n";
// 		std::cout << "\t" << "redirection.url : [" << locs[i].redirection.url << "]\n";
// 		std::cout << "\t" << "redirection.status : [" << locs[i].redirection.status << "]\n";
// 	}
// }

// void error_pages(std::vector<ErrorPage> err)
// {
// 	std::cout << "\n** ERROR PAGES " << " **\n\n";
// 	for (size_t i = 0; i < err.size(); i++)
// 	{
// 		std::cout << "\t" << "[" << err[i].status << " | " << err[i].path << "]\n";
		
// 	}
// }


// int main()
// {
// 	ConfigFile config_file("webserv.conf");

// 	for (size_t i = 0; i < config_file.configuration.size(); i++)
// 	{
// 		std::cout << "\n** SERVER " << i << " **\n\n";
// 		std::cout << "host : [" << config_file.configuration[i].host << "]\n";
// 		std::cout << "client_max_body_size : [" << config_file.configuration[i].client_max_body_size << "]\n";
// 		std::cout << "post : ["; test_list(config_file.configuration[i].port); std::cout << "]\n";
// 		std::cout << "server_name : ["; test_list(config_file.configuration[i].server_name); std::cout << "]\n";

// 		locations(config_file.configuration[i].location);
// 		error_pages(config_file.configuration[i].error_page);
// 	}
// 	return 0;
// }
