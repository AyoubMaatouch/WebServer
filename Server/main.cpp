#include "socket.hpp"
#include "Server.hpp"

int main()
{
	try
	{
		/***
		 * HOW THE PROGRAM WORKS:
		 * first: we  initialize the servers with the host and port from config file
		 * then we create the sockets and bind it to the port
		 * then we listen to the port
		 * and we accept the connection and assign it to a new fd and we aling to the fd to request object using a map 
		 *  the create a response object based on the request object
		 * END
		 * PS: don't forget about the CGI
		 * */
		
		// ConfigFile configuration("./webserv.conf");
		_server server("127.0.0.1", 8080);

		Mysocket obj;
		obj.start_server(server);
		// obj.start_server(AF_INET, SOCK_STREAM, 0, 8088, 128);
		// obj.accept_connection();
		
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}


//* Sockets
//https://www.geeksforgeeks.org/socket-programming-cc/#:~:text=Socket%20programming%20is%20a%20way,reaches%20out%20to%20the%20server.

