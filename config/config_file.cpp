#include "library.hpp"

/*
**
* DEFAULT
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
{
	for (size_t i = 0; i < conf.size(); i++)
	{
		for (size_t j = 0; j < conf[i]->location.size(); j++)
		{
			delete conf[i]->location[j];
		}
		delete conf[i];
	}
}

/*
**
* CONFIGURATION
**
*/

ConfigFile::ConfigFile(const char *file_path)
{
	Server *server;
	Location *location;
	ErrorPage *error_page;
	std::string line;
	std::ifstream file(file_path);
	bool is_server = false, is_location = false, is_error_page = false;

	while (std::getline(file, line))
	{
		// get the key without whitespace
		std::string key = line.substr(0, line.find(':'));
		std::string value = line.substr(line.find(':') + 1, line.size());

		// clean the key
		key.erase(std::remove(key.begin(), key.end(), ' '), key.end());

		// add info
		if (key == "server")
		{
			server = new Server();
			conf.push_back(server);
			is_error_page = false;
		}
		else if (key == "\tlocation")
		{
			location = new Location();
			server->location.push_back(location);
			is_error_page = false;
		}
		else if (key == "\terrors_pages")
		{
			is_error_page = true;
		}
		else if (key == "\tserver_name")
		{
			list(value, &server->server_name, ' ');
		}
		else if (key == "\t\tindex")
		{
			list(value, &location->index, ' ');
		}
		else if (key == "\tport")
		{
			list(value, &server->port, ' ');
		}
		else
		{
			value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
			if (key == "\thost")
			{
				server->host = value;
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
			else if (key == "\t\t\tstatus")
			{
				location->redirection.status = ft_atoi(value);
			}
			else if (key == "\t\t\turl")
			{
				location->redirection.url = value;
			}
			else if (is_error_page)
			{
				key.erase(std::remove(key.begin(), key.end(), '\t'), key.end());
				int status = ft_atoi(key);
				
				if (status < 0 || status > 599)
					throw ConfigFile::SyntaxError();
				
				error_page = new ErrorPage();
				error_page->status = status;
				error_page->path = value;
				server->error_page.push_back(error_page);
			}
			else
				throw ConfigFile::SyntaxError();
		}
	}
	file.close();
}
