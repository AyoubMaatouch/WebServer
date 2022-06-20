#include "library.hpp"

/*
**
* DEFAULT
**
*/

// Redirection
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

// Cgi
Cgi::Cgi()
	: extension(), path()
{
}

Cgi &Cgi::operator=(const Cgi &copy)
{
	extension = copy.extension;
	path = copy.path;

	return (*this);
}

// ErrorPage
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

// Location
Location::Location()
	: path(),
	  root(),
	  index(),
	  upload(),
	  cgi(),
	  auto_index(),
	  redirection(),
	  is_redirect(false)
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
	return (*this);
}

// Server
Server::Server()
	: host(),
	  port(),
	  server_name(),
	  client_max_body_size(),
	  location(),
	  error_page()
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
	return (*this);
}

// ------------------------------------

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
			std::string key = line.substr(0, line.find(':'));

			if (key == "server" || is_line)
			{
				is_line = false;
				set_server();
			}
			else
				throw std::runtime_error("Uknown key");
		}
	}
	check_all();
}

void ConfigFile::set_server(void)
{
	Server server;
	configuration.push_back(server);

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
				duplicate_key((configuration.back()).server_name.size());
				list(value, &(configuration.back()).server_name, ' ');
			}
			else if (key == "\tport")
			{
				duplicate_key((configuration.back()).port.size());
				list(value, &(configuration.back()).port, ' ');
			}
			else if (key == "\thost")
			{
				duplicate_key((configuration.back()).host != "");
				(configuration.back()).host = value;
			}
			else if (key == "\tclient_max_body_size")
			{
				duplicate_key((configuration.back()).client_max_body_size != 0);
				long long holder = stol(value);
				if (holder < 0)
					throw std::runtime_error("client_max_body_size : Syntax Error");
				else
					(configuration.back()).client_max_body_size = holder;
			}
			else if (key == "\tlocation")
			{
				set_location();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw std::runtime_error("Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 0)
					return;
			}
			else if (key == "\terror_page")
			{
				duplicate_key((configuration.back()).error_page.size());
				set_error_page();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 1)
					throw std::runtime_error("Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 0)
					return;
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

void ConfigFile::set_location(void)
{
	Location location;
	(configuration.back()).location.push_back(location);

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
				duplicate_key(((configuration.back()).location.back()).index.size());
				list(value, &((configuration.back()).location.back()).index, ' ');
			}
			else if (key == "\t\tpath")
			{
				duplicate_key(((configuration.back()).location.back()).path != "");
				((configuration.back()).location.back()).path = value;
			}
			else if (key == "\t\troot")
			{
				duplicate_key(((configuration.back()).location.back()).root != "");
				((configuration.back()).location.back()).root = value;
			}
			else if (key == "\t\tupload")
			{
				duplicate_key(((configuration.back()).location.back()).upload != "");
				((configuration.back()).location.back()).upload = value;
			}
			else if (key == "\t\tcgi")
			{
				duplicate_key((configuration.back()).error_page.size());
				set_cgi();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw std::runtime_error("Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 1)
					return;
			}
			else if (key == "\t\tauto_index")
			{
				if (value == "on")
					((configuration.back()).location.back()).auto_index = true;
				else if (value == "off")
					((configuration.back()).location.back()).auto_index = false;
				else
					throw std::runtime_error("auto index : Syntax Error");
			}
			else if (key == "\t\tredirection")
			{
				((configuration.back()).location.back()).is_redirect = true;
				duplicate_key(((configuration.back()).location.back()).redirection.status != 0 && ((configuration.back()).location.back()).redirection.url != "");
				set_redirection();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw std::runtime_error("Syntax Error");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) <= 1)
					return;
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
				duplicate_key(redirection.url != "");
				redirection.url = value;
			}
			else if (key == "\t\t\tstatus")
			{
				duplicate_key(redirection.status != 0);
				redirection.status = status_code(value);
			}
			else
			{
				previous_line = line;
				is_line = true;
				((configuration.back()).location.back()).redirection = redirection;
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
					duplicate_key(holder == (configuration.back()).error_page[i].status);

				error_page.status = holder;

				if (value.size() == 0)
					throw std::runtime_error("error page path: path not found for status code : " + key);
				error_page.path = value;

				(configuration.back()).error_page.push_back(error_page);
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

void ConfigFile::set_cgi(void)
{
	std::string line;

	while (read_line(line))
	{
		if (!is_white_space(line))
		{
			Cgi cgi;
			std::string key = line.substr(0, line.find(':'));
			std::string value = clean_whitespace(line.substr(line.find(':') + 1, line.size()));

			if (which_level(key) == 3)
			{
				key = clean_whitespace(key);
				if (!allowed_extension(key))
					throw std::runtime_error("Syntax Error");
				for (size_t i = 0; i < ((configuration.back()).location.back()).cgi.size(); i++)
					duplicate_key(key == ((configuration.back()).location.back()).cgi[i].extension);
				cgi.extension = key;
				if (value.size() == 0)
					throw std::runtime_error("CGI path : " + key + " : Syntax Error");
				cgi.path = clean_whitespace(value);
				(configuration.back()).location.back().cgi.push_back(cgi);
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
* CHECK
**
*/

void ConfigFile::check_all(void)
{
	if (configuration.size() == 0)
		throw std::runtime_error("configuration file is empty");
	for (size_t i = 0; i < configuration.size(); i++)
	{
		check_server(configuration[i]);
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
		throw std::runtime_error("Host is missing");
	if (server.port.size() == 0)
		throw std::runtime_error("Post is missing");
	if (server.server_name.size() == 0)
		throw std::runtime_error("Server name is missing");
	if (server.client_max_body_size == 0)
		server.client_max_body_size = std::numeric_limits<long long>::max();
	if (server.location.size() == 0)
		throw std::runtime_error("Server location is missing");
	if (server.error_page.size() == 0)
		throw std::runtime_error("Server error pages is missing");
}

void ConfigFile::check_location(Location location)
{
	if (location.path == "")
		throw std::runtime_error("Location path is missing");
	if (location.upload == "")
		throw std::runtime_error("Location upload path is missing");
	if (location.cgi.size() == 0)
		throw std::runtime_error("Location CGI path is missing");
	if (location.root == "")
		throw std::runtime_error("Location root path is missing");
	if (location.index.size() == 0)
		throw std::runtime_error("Location index is missing");
}

void ConfigFile::check_redirection(Redirection redirection)
{
	if (redirection.status == 0)
		throw std::runtime_error("Redirection status is missing");
	if (redirection.url == "")
		throw std::runtime_error("Redirection url is missing");
}

void ConfigFile::duplicate_key(bool value)
{
	if (value)
	{
		throw std::runtime_error("duplicate key");
	}
}
