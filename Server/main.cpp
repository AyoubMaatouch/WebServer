#include "socket.hpp"
#include "Server.hpp"

int main()
{
	try
	{
		// idea: create an object for each conn
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

