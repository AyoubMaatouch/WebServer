#include "library.hpp"

/*
**
* DEFAULT
**
*/

Server::Server() { client_max_body_size = 0; }

ErrorPage::ErrorPage() { status = 0; }

Redirection::Redirection() { status = 0; }

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

ConfigFile::ConfigFile(const char *file_path)
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
				throw ConfigFile::Error("Uknown key");
		}
	}
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

				int holder = ft_atoi(value);
				if (holder < 0)
					throw ConfigFile::Error("client_max_body_size : Syntax Error");
				(configuration.back()).client_max_body_size = holder;
			}
			else if (key == "\tlocation")
			{
				set_location();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 1)
					throw ConfigFile::Error("Syntax Error : (" + clean_whitespace(previous_line.substr(0, previous_line.find(':'))) + ")");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 0)
					return;
			}
			else if (key == "\terror_page")
			{
				duplicate_key((configuration.back()).error_page.size());
				set_error_page();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 1)
					throw ConfigFile::Error("Syntax Error : (" + clean_whitespace(previous_line.substr(0, previous_line.find(':'))) + ")");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) == 0)
				{
					check_server();
					return;
				}
			}
			else
			{
				previous_line = line;
				is_line = true;
				check_server();
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
				duplicate_key(((configuration.back()).location.back()).cgi != "");
				((configuration.back()).location.back()).cgi = value;
			}
			else if (key == "\t\tauto_index")
			{
				if (value == "on")
					((configuration.back()).location.back()).auto_index = true;
				else if (value == "off")
					((configuration.back()).location.back()).auto_index = false;
				else
					throw ConfigFile::Error("auto index : Syntax Error");
			}
			else if (key == "\t\tredirection")
			{
				duplicate_key(((configuration.back()).location.back()).redirection.status != 0 && ((configuration.back()).location.back()).redirection.url != "");
				set_redirection();
				if (which_level(previous_line.substr(0, previous_line.find(':'))) > 2)
					throw ConfigFile::Error("Syntax Error : (" + clean_whitespace(previous_line.substr(0, previous_line.find(':'))) + ")");
				if (which_level(previous_line.substr(0, previous_line.find(':'))) <= 1)
				{
					check_location();
					return;
				}
			}
			else
			{
				previous_line = line;
				is_line = true;
				check_location();
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
				int holder = ft_atoi(value);
				if (holder < 0)
					throw ConfigFile::Error("redirection status : Syntax Error");
				redirection.status = holder;
			}
			else
			{
				previous_line = line;
				is_line = true;
				((configuration.back()).location.back()).redirection = redirection;
				check_redirection();
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
