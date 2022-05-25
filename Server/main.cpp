#include "socket.hpp"

int main()
{
	try
	{
		// idea: create an object for each conn
		Mysocket obj(AF_INET, SOCK_STREAM, 0, 8080, 128);
		
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}


//* Sockets
//https://www.geeksforgeeks.org/socket-programming-cc/#:~:text=Socket%20programming%20is%20a%20way,reaches%20out%20to%20the%20server.

