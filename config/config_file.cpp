#include "config_file.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

/*
**
* Default
**
*/

ConfigFile::ConfigFile()
{ // nothing
}

ConfigFile::ConfigFile(const ConfigFile &copy)
{
	(void)copy;
}

ConfigFile &ConfigFile::operator=(const ConfigFile &copy)
{
	(void)copy;
	return (*this);
}

ConfigFile::~ConfigFile()
{ // nothing
}

/*
**
* Configuration
**
*/

void list(std::string value, std::vector<std::string> *ptr)
{
	std::stringstream ss_value(value);
	while (std::getline(ss_value, value, ' '))
	{
		if (value.size())
		{
			ptr->push_back(value);
		}
	}
}

int ft_atoi(std::string num)
{
	int i = 0;

	for (std::string::iterator it = num.begin(); it != num.end(); it++)
	{
		i += (*it - 48);
		i *= 10;
	}
	return (i / 10);
}

bool ConfigFile::set_server(std::string line, Server *server)
{
	std::string key = line.substr(0, line.find(':'));
	std::string value = line.substr(line.find(':') + 1, line.size());

	if (key == "\tserver_name")
	{
		list(value, &server->server_name);
	}
	else
	{
		value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
		if (key == "\thost")
		{
			server->host = value;
		}
		else if (key == "\tport")
		{
			server->port = value;
		}
		else if (key == "\tclient_max_body_size")
		{
			int holder = ft_atoi(value);
			if (holder < 0)
			{
				throw ConfigFile::SyntaxError();
			}
			server->client_max_body_size = holder;
		}
		else
		{
			// conf.push_back(*server);
			return true;
		}
	}
	return false;
}

bool ConfigFile::set_location(std::string line, Location *location)
{
	std::string key = line.substr(0, line.find(':'));
	std::string value = line.substr(line.find(':') + 1, line.size());

	if (key == "\t\tindex")
	{
		list(value, &location->index);
	}
	else
	{
		value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
		if (key == "\t\tpath")
		{
			location->path = value;
		}
		else if (key == "\t\troot")
		{
			location->root = value;
		}
		else if (key == "\t\tupload")
		{
			location->upload = value;
		}
		else if (key == "\t\tcgi")
		{
			location->cgi = value;
		}
		else if (key == "\t\tauto_index")
		{
			if (value == "on")
				location->auto_index = true;
			else if (value == "off")
				location->auto_index = false;
			else
				throw ConfigFile::SyntaxError();
		}
		else if (key == "\t\tredirection")
		{
			std::stringstream ss_value(value);
			ss_value >> location->redirection.status >> location->redirection.url;
		}
		else
		{
			return true;
		}
	}
	return false;
}

ConfigFile::ConfigFile(const char *file_path)
{
	Server *server;
	Location *location;
	std::string line;
	std::ifstream file(file_path);
	bool is_server = false, is_location = false;

	while (std::getline(file, line))
	{
		// get the key without whitespace
		std::string key = line.substr(0, line.find(':'));
		key.erase(std::remove(key.begin(), key.end(), ' '), key.end());

		// add information to a server
		if (is_server)
		{
			if (key == "\tlocation")
			{
				location = new Location();
				server->location.push_back(*location);
				is_location = true;
			}
			else if (is_location)
			{
				if (set_location(line, location))
				{
					is_location = false;
				}
			}
			else
			{
				if (set_server(line, server))
				{
					is_server = false;
				}
			}
		}
		else if (key == "server")
		{
			server = new Server();
			conf.push_back(*server);
			is_server = true;
		}
		else
		{
			key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
			key.erase(std::remove(key.begin(), key.end(), '\t'), key.end());
			std::cout << key << "\n";
			if (key.size())
				throw ConfigFile::SyntaxError();
		}
	}

	file.close();
}
