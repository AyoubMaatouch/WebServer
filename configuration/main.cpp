#include "library.hpp"
#include "config_file.hpp"

int main()
{
	ConfigFile conf("webserv.conf");

	for (size_t i = 0; i < conf.configuration.size(); i++)
	{
		for (size_t j = 0; j < conf.configuration[i].port.size(); j++)
		{
			std::cout << conf.configuration[i].port[j] << std::endl;
		}
	}
	return 0;
}
