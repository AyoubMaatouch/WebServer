#include "library.hpp"

/*
**
* DEFAULT
**
*/

ConfigFile::ConfigFile(void) {}

ConfigFile::ConfigFile(const ConfigFile &copy) { (void)copy; }

ConfigFile &ConfigFile::operator=(const ConfigFile &copy)
{
	(void)copy;
	return (*this);
}

ConfigFile::~ConfigFile(void) {}

/*
**
* CONFIGURATION
**
*/

ConfigFile::ConfigFile(const char *file_path)
{
	file.open(file_path);
	is_server = false;

	std::string line;

	while (std::getline(file, line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));

			if (key == "server" || is_server)
			{
				is_server = false;
				set_server();
			}
			else
				throw ConfigFile::Error("Uknown key");
		}
	}
}

void ConfigFile::set_server(void)
{
	Server server;
	configuration.push_back(server);

	is_location = false;

	std::string line;

	while (std::getline(file, line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (key == "\tserver_name")
			{
				duplicate_key(server.server_name.size());
				list(value, &server.server_name, ' ');
			}
			else if (key == "\tport")
			{
				duplicate_key(server.port.size());
				list(value, &server.port, ' ');
			}

			else if (key == "\thost")
			{
				duplicate_key(server.host != "");
				server.host = value;
			}
			else if (key == "\tclient_max_body_size")
			{
				duplicate_key(server.client_max_body_size != 0);

				int holder = ft_atoi(value);
				if (holder < 0)
					throw ConfigFile::Error("client_max_body_size : Syntax Error");
				server.client_max_body_size = holder;
			}
			else if (key == "\tlocation" || is_location)
			{
				is_location = false;
				set_location();
			}
			else if (key == "\terror_page")
			{
				duplicate_key(server.error_page.size());
				set_error_page();
			}
			else
			{
				if (key != "server")
					throw ConfigFile::Error(clean_whitespace(key) + " Uknown key");
				else
				{
					check_server();
					is_server = true;
				}
			}
		}
	}
}

void ConfigFile::set_location(void)
{
	Location location;
	(configuration.back()).location.push_back(location);

	std::string line;

	while (std::getline(file, line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (key == "\t\tindex")
			{
				duplicate_key(location.index.size());
				list(value, &location.index, ' ');
			}
			else if (key == "\t\tpath")
			{
				duplicate_key(location.path != "");
				location.path = value;
			}
			else if (key == "\t\troot")
			{
				duplicate_key(location.root != "");
				location.root = value;
			}
			else if (key == "\t\tupload")
			{
				duplicate_key(location.upload != "");
				location.upload = value;
			}
			else if (key == "\t\tcgi")
			{
				duplicate_key(location.cgi != "");
				location.cgi = value;
			}
			else if (key == "\t\tauto_index")
			{
				if (value == "on")
					location.auto_index = true;
				else if (value == "off")
					location.auto_index = false;
				else
					throw ConfigFile::Error("auto index : Syntax Error");
			}
			else if (key == "\t\tredirection")
			{
				duplicate_key(location.redirection.status != 0 && location.redirection.url != "");
				set_redirection();
			}
			else
			{
				if (key != "\tlocation" && key != "server")
					throw ConfigFile::Error(clean_whitespace(key) + " Uknown key");
				else
				{
					check_location();
					is_location = true;
				}
			}
		}
	}
}

void ConfigFile::set_redirection(void)
{
	Redirection redirection;
	std::string line;

	while (std::getline(file, line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (key == "\t\t\turl")
			{
				duplicate_key(redirection.url != "");
				redirection.url = value;
			}
			else if (key == "\t\t\tstatus")
			{
				duplicate_key(redirection.status != 0);

				int holder = ft_atoi(value);
				if (holder < 0)
					throw ConfigFile::Error("redirection status : Syntax Error");
				redirection.status = holder;
			}
			else
			{
				if (key != "\tlocation" && key != "server")
					throw ConfigFile::Error(clean_whitespace(key) + " Uknown key");
				else
				{
					((configuration.back()).location.back()).redirection = redirection;
					check_redirection();
				}
			}
		}
	}
}

void ConfigFile::set_error_page(void)
{
	std::string line;

	while (std::getline(file, line))
	{
		if (!is_white_space(line))
		{
			ErrorPage error_page;
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (is_aligned(key) && is_digit(clean_whitespace(key)))
			{
				int holder = ft_atoi(clean_whitespace(key));

				for (size_t i = 0; i < (configuration.back()).error_page.size(); i++)
					duplicate_key(holder == (configuration.back()).error_page[i].status);

				if (holder < 0)
					throw ConfigFile::Error("error page status : Syntax Error");
				error_page.status = holder;
				
				if (value.size() == 0)
					throw ConfigFile::Error("error page path: Syntax Error");
				error_page.path = value;

				(configuration.back()).error_page.push_back(error_page);
			}
			else
				throw ConfigFile::Error("error page: Syntax Error");
		}
	}
}

void ConfigFile::check_redirection(void)
{
	Redirection redirection = ((configuration.back()).location.back()).redirection;

	if (redirection.status == 0)
		throw ConfigFile::Error("Redirection status is missing");
	if (redirection.url == "")
		throw ConfigFile::Error("Redirection url is missing");
}

void ConfigFile::check_location(void)
{
	Location location = (configuration.back()).location.back();

	if (location.path == "")
		throw ConfigFile::Error("Location path is missing");
	if (location.upload == "")
		throw ConfigFile::Error("Location upload path is missing");
	if (location.cgi == "")
		throw ConfigFile::Error("Location CGI path is missing");
	if (location.root == "")
		throw ConfigFile::Error("Location root path is missing");
	if (location.index.size() == 0)
		throw ConfigFile::Error("Location index is missing");
	check_redirection();
}

void ConfigFile::check_server(void)
{
	Server server = configuration.back();

	if (server.host == "")
		throw ConfigFile::Error("Host is missing");
	if (server.port.size() == 0)
		throw ConfigFile::Error("Post is missing");
	if (server.server_name.size() == 0)
		throw ConfigFile::Error("Server name is missing");
	if (server.client_max_body_size == 0)
		throw ConfigFile::Error("Server client max body size is missing");
	if (server.location.size() == 0)
		throw ConfigFile::Error("Server location is missing");
	if (server.error_page.size() == 0)
		throw ConfigFile::Error("Server error pages is missing");
}

void ConfigFile::duplicate_key(bool value)
{
	if (value)
		throw ConfigFile::Error("duplicate key");
}

// ConfigFile::ConfigFile(const char *file_path)
// {
// 	Server *server;
// 	Location *location;
// 	ErrorPage *error_page;
// 	std::string line;
// 	std::ifstream file(file_path);
// 	bool is_server = false, is_location = false, is_error_page = false;

// 	while (std::getline(file, line))
// 	{
// 		// get the key without whitespace
// 		std::string key = line.substr(0, line.find(':'));
// 		std::string value = line.substr(line.find(':') + 1, line.size());

// 		// clean the key
// 		key.erase(std::remove(key.begin(), key.end(), ' '), key.end());

// 		// add info
// 		if (key == "server")
// 		{
// 			server = new Server();
// 			conf.push_back(server);
// 			is_error_page = false;
// 		}
// 		else if (key == "\tlocation")
// 		{
// 			location = new Location();
// 			server->location.push_back(location);
// 			is_error_page = false;
// 		}
// 		else if (key == "\terrors_pages")
// 		{
// 			is_error_page = true;
// 		}
// 		else if (key == "\tserver_name")
// 		{
// 			list(value, &server->server_name, ' ');
// 		}
// 		else if (key == "\t\tindex")
// 		{
// 			list(value, &location->index, ' ');
// 		}
// 		else if (key == "\tport")
// 		{
// 			list(value, &server->port, ' ');
// 		}
// 		else
// 		{
// 			value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
// 			if (key == "\thost")
// 			{
// 				server->host = value;
// 			}
// 			else if (key == "\tclient_max_body_size")
// 			{
// 				int holder = ft_atoi(value);
// 				if (holder < 0)
// 				{
// 					throw ConfigFile::Error("");
// 				}
// 				server->client_max_body_size = holder;
// 			}
// 			if (key == "\t\tpath")
// 			{
// 				location->path = value;
// 			}
// 			else if (key == "\t\troot")
// 			{
// 				location->root = value;
// 			}
// 			else if (key == "\t\tupload")
// 			{
// 				location->upload = value;
// 			}
// 			else if (key == "\t\tcgi")
// 			{
// 				location->cgi = value;
// 			}
// 			else if (key == "\t\tauto_index")
// 			{
// 				if (value == "on")
// 					location->auto_index = true;
// 				else if (value == "off")
// 					location->auto_index = false;
// 				else
// 					throw ConfigFile::Error("");
// 			}
// 			else if (key == "\t\t\tstatus")
// 			{
// 				location->redirection.status = ft_atoi(value);
// 			}
// 			else if (key == "\t\t\turl")
// 			{
// 				location->redirection.url = value;
// 			}
// 			else if (is_error_page)
// 			{
// 				key.erase(std::remove(key.begin(), key.end(), '\t'), key.end());
// 				int status = ft_atoi(key);

// 				if (status < 0 || status > 599)
// 					throw ConfigFile::Error("");

// 				error_page = new ErrorPage();
// 				error_page->status = status;
// 				error_page->path = value;
// 				server->error_page.push_back(error_page);
// 			}
// 			else
// 				throw ConfigFile::Error("");
// 		}
// 	}
// 	file.close();
// }
