#include "socket.hpp"
// #include "Server.hpp"
#include "../configuration/config_file.hpp"

int main(int ac, char** av)
{
	if (ac == 2)
	{

		try
		{
		
			ConfigFile _con(av[1]);
			std::vector<Server > servers = _con.configuration;
			Mysocket obj;
			std::cout << "Njinx is running" << std::endl;
			signal(SIGPIPE, SIG_IGN);
			obj.start_server(servers);
			
					
			return (0);
			
		}
		catch(const std::exception &e)
		{
			std::cerr << e.what() << std::endl;
		}

	}
	return (0);
}

 //* Notion 
 //* https://www.notion.so/maroislife/Web-Server-d3c265446b2b488cadfe7204fc9933d6

//* Sockets
//* https://www.geeksforgeeks.org/socket-programming-cc/#:~:text=Socket%20programming%20is%20a%20way,reaches%20out%20to%20the%20server.
