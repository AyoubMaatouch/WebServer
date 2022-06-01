#pragma one

#include <iostream>
#include <fstream>
#include <vector>

struct Redirection
{
	int status;
	std::string url;
};

struct Location
{
	std::string path;
	std::string root;
	std::vector<std::string> index;
	std::string upload;
	std::string cgi;
	bool auto_index;
	Redirection redirection;
};

struct Server
{
	std::string host;
	std::string port;
	std::vector<std::string> server_name;
	int client_max_body_size;
	std::vector<Location> location;
};

class ConfigFile
{
private:
	ConfigFile();
	ConfigFile(const ConfigFile &copy);
	ConfigFile &operator=(const ConfigFile &copy);

	bool set_server(std::string line, Server *server);
	bool set_location(std::string line, Location *location);

	// exceptions
	class SyntaxError : public std::exception
	{
	public:
		virtual const char *what() const throw()
		{
			return ("syntax error");
		}
	};

public:
	~ConfigFile();
	ConfigFile(const char* file_path);
	std::vector<Server> conf;
};