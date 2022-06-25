#include "library.hpp"

/*
**
* DEFAULT
**
*/

// *********************** Redirection
Redirection::Redirection()
	: status(), url()
{
}

Redirection &Redirection::operator=(const Redirection &copy)
{
	status = copy.status;
	url = copy.url;

	return (*this);
}

// *********************** ErrorPage
ErrorPage::ErrorPage()
	: status(), path()
{
}

ErrorPage &ErrorPage::operator=(const ErrorPage &copy)
{
	status = copy.status;
	path = copy.path;
	return (*this);
}

// *********************** Location
Location::Location()
	: path(),
	  root(),
	  index(),
	  upload(),
	  cgi(),
	  auto_index(),
	  redirection(),
	  is_redirect(false),
	  is_cgi(false)
{
}

Location &Location::operator=(const Location &copy)
{
	path = copy.path;
	root = copy.root;
	index = copy.index;
	upload = copy.upload;
	cgi = copy.cgi;
	auto_index = copy.auto_index;
	redirection = copy.redirection;
	is_redirect = copy.is_redirect;
	is_cgi = copy.is_cgi;
	return (*this);
}

// *********************** Server
Server::Server()
	: host(),
	  port(),
	  server_name(),
	  client_max_body_size(std::numeric_limits<long long>::max()),
	  location(),
	  error_page(),
	  is_error_page(false)
{
}

Server &Server::operator=(const Server &copy)
{
	host = copy.host;
	port = copy.port;
	server_name = copy.server_name;
	client_max_body_size = copy.client_max_body_size;
	location = copy.location;
	error_page = copy.error_page;
	is_error_page = copy.is_error_page;
	return (*this);
}

// ********************************************************************

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
************************************************** CONFIGURATION
**
*/

bool ConfigFile::read_line(std::string &line)
{
	if (is_eof)
		return false;
	else if (is_line)
		line = previous_line;
	else if ((std::getline(file, line)).eof())
		is_eof = true;
	return true;
}

ConfigFile::ConfigFile(std::string file_path)
{
	file.open(file_path);

	std::string line;
	is_line = false;
	is_eof = false;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			if (line.substr(0, line.find(':')) == "server" || is_line)
			{
				is_line = false;
				set_server();
			}
			else
				throw std::runtime_error("[ ERROR ] - Uknown key");
		}
	}
	check_all();
}

void ConfigFile::set_server(void)
{
	configuration.push_back(Server());
	
	std::string line;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			is_line = false;

			if (key == "\tserver_name")
			{
				check_for_error(configuration.back().server_name.size(), value);
				list(value, &configuration.back().server_name, ' ');
			}
			else if (key == "\tport")
			{
				check_for_error(configuration.back().port.size(), value);
				list(value, &configuration.back().port, ' ');
			}
			else if (key == "\thost")
			{
				check_for_error(configuration.back().host != "", value);
				configuration.back().host = value;
			}
			else if (key == "\tclient_max_body_size")
			{
				check_for_error(configuration.back().client_max_body_size != 0, value);
				long long holder = stol(value);
				if (holder < 0)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				else
					configuration.back().client_max_body_size = holder;
			}
			else if (key == "\tlocation")
			{
				set_location();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 0)
					return;
			}
			else if (key == "\terror_page")
			{
				duplicate_key(configuration.back().is_error_page);
				configuration.back().is_error_page = true;
				set_error_page();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 1)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 0)
					return;
			}
			else
			{
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 1)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				previous_line = line;
				is_line = true;
				return;
			}
		}
	}
}

void ConfigFile::set_location(void)
{
	configuration.back().location.push_back(Location());

	std::string line;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			is_line = false;

			if (key == "\t\tindex")
			{
				check_for_error(configuration.back().location.back().index.size(), value);
				list(value, &configuration.back().location.back().index, ' ');
			}
			else if (key == "\t\tallowed_method")
			{
				check_for_error(configuration.back().location.back().allowed_method.size(), value);

				std::vector<std::string> methods;
				list(value, &methods, ' ');
				configuration.back().location.back().allowed_method["GET"] = false;
				configuration.back().location.back().allowed_method["POST"] = false;
				configuration.back().location.back().allowed_method["DELETE"] = false;
				
				for (size_t i = 0; i < methods.size(); i++)
				{
					methods[i] = upperCase(methods[i]);
					if (configuration.back().location.back().allowed_method[methods[i]])
						throw std::runtime_error("[ ERROR ] - allowed_method - duplicated method");
					configuration.back().location.back().allowed_method[methods[i]] = true;
				}

				if (!allowed_methods(methods))
					throw std::runtime_error("[ ERROR ] - allowed_method - method not allowed");

			}
			else if (key == "\t\tpath")
			{
				check_for_error(configuration.back().location.back().path != "", value);
				configuration.back().location.back().path = value;
			}
			else if (key == "\t\troot")
			{
				check_for_error(configuration.back().location.back().root != "", value);
				configuration.back().location.back().root = value;
			}
			else if (key == "\t\tupload")
			{
				check_for_error(configuration.back().location.back().upload != "", value);
				configuration.back().location.back().upload = value;
			}
			else if (key == "\t\tcgi")
			{
				duplicate_key(configuration.back().location.back().is_cgi);
				configuration.back().location.back().is_cgi = true;
				set_cgi();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 1)
					return;
			}
			else if (key == "\t\tauto_index")
			{
				if (value == "on")
					configuration.back().location.back().auto_index = true;
				else if (value == "off")
					configuration.back().location.back().auto_index = false;
				else
					throw std::runtime_error("[ ERROR ] - Syntax Error");
			}
			else if (key == "\t\tredirection")
			{
				duplicate_key(configuration.back().location.back().is_redirect);
				configuration.back().location.back().is_redirect = true;
				set_redirection();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) <= 1)
					return;
			}
			else
			{
				if (which_level(line.substr(0, line.find(':'))) == 2)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				previous_line = line;
				is_line = true;
				return;
			}
		}
	}
}

void ConfigFile::set_redirection(void)
{
	Redirection redirection;
	std::string line;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (key == "\t\t\turl")
			{
				check_for_error(redirection.url != "", value);
				redirection.url = value;
			}
			else if (key == "\t\t\tstatus")
			{
				check_for_error(redirection.status != 0, value);
				redirection.status = status_code(value);
			}
			else
			{
				if (which_level(line.substr(0, line.find(':'))) == 3)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				previous_line = line;
				is_line = true;
				configuration.back().location.back().redirection = redirection;
				return;
			}
		}
	}
}

void ConfigFile::set_error_page(void)
{
	std::string line;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			ErrorPage error_page;
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (which_level(key) == 2)
			{
				key = clean_whitespace(key);
				int holder = status_code(key);

				for (size_t i = 0; i < (configuration.back()).error_page.size(); i++)
					duplicate_key(holder == configuration.back().error_page[i].status);

				error_page.status = holder;

				if (value.size() == 0)
					throw std::runtime_error("[ ERROR ] - error_page - path is missing for the status code " + key);
				error_page.path = value;

				configuration.back().error_page.push_back(error_page);
			}
			else
			{
				if (which_level(line.substr(0, line.find(':'))) == 2)
					throw std::runtime_error("[ ERROR ] - Syntax Error");
				previous_line = line;
				is_line = true;
				return;
			}
		}
	}
}

void ConfigFile::set_cgi(void)
{
	std::string line;
	std::map<std::string, std::string> &cgi = configuration.back().location.back().cgi;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (which_level(key) == 3)
			{
				key = clean_whitespace(key);
				if (!allowed_extension(key))
					throw std::runtime_error("[ ERROR ] - cgi - extension not allowed - " + key);
				
				check_for_error(cgi.count(key), value);
				configuration.back().location.back().cgi[key] = value;
			}
			else
			{
				previous_line = line;
				is_line = true;
				return;
			}
		}
	}
}

/*
**
******************************************* CHECK
**
*/

void ConfigFile::check_all(void)
{
	if (configuration.size() == 0)
		throw std::runtime_error("[ ERROR ] - configuration file is empty");
	for (size_t i = 0; i < configuration.size(); i++)
	{
		check_server(configuration[i]);
		if (configuration[i].location[0].path != "/")
			throw std::runtime_error("[ ERROR ] - location - the first location is not root");
		for (size_t j = 0; j < configuration[i].location.size(); j++)
		{
			check_location(configuration[i].location[j]);
			if (configuration[i].location[j].is_redirect)
				check_redirection(configuration[i].location[j].redirection);
		}
	}
}

void ConfigFile::check_server(Server server)
{
	if (server.host == "")
		throw std::runtime_error("[ ERROR ] - server - host is missing");
	if (server.port.size() == 0)
		throw std::runtime_error("[ ERROR ] - server - post is missing");
	if (server.server_name.size() == 0)
		throw std::runtime_error("[ ERROR ] - server - server_name is missing");
	if (server.server_name.size() > 1)
		throw std::runtime_error("[ ERROR ] - server - server_name has more than one entry");
	if (server.location.size() == 0)
		throw std::runtime_error("[ ERROR ] - server - location is missing");
	if (server.error_page.size() == 0)
		throw std::runtime_error("[ ERROR ] - server - error_pages is missing");
}

void ConfigFile::check_location(Location location)
{
	if (location.path == "")
		throw std::runtime_error("[ ERROR ] - location - path is missing");
	if (location.path[0] != '/')
		throw std::runtime_error("[ ERROR ] - location - path is relative");
	if (location.upload == "")
		throw std::runtime_error("[ ERROR ] - location - upload is missing");
	if (location.cgi.size() == 0)
		throw std::runtime_error("[ ERROR ] - location - cgi is missing");
	if (location.root == "")
		throw std::runtime_error("[ ERROR ] - location - root is missing");
	if (location.root[0] != '/')
		throw std::runtime_error("[ ERROR ] - location - root is relative");
	if (location.index.size() == 0)
		throw std::runtime_error("[ ERROR ] - location - index is missing");
	if (location.allowed_method.size() == 0)
		throw std::runtime_error("[ ERROR ] - location - allowed_method is missing");
}

void ConfigFile::check_redirection(Redirection redirection)
{
	if (redirection.status == 0)
		throw std::runtime_error("[ ERROR ] - redirection - status is missing");
	if (redirection.url == "")
		throw std::runtime_error("[ ERROR ] - redirection - url is missing");
}

/*
**
******************************************* ERRO CHECK
**
*/

void ConfigFile::check_for_error(bool duplicate, std::string value)
{
	duplicate_key(duplicate);
	empty_value(value.size() == 0);
}

void ConfigFile::duplicate_key(bool duplicate)
{
	if (duplicate)
		throw std::runtime_error("[ ERROR ] - duplicate key");
}

void ConfigFile::empty_value(bool empty)
{
	if (empty)	
		throw std::runtime_error("[ ERROR ] - empty value");
}