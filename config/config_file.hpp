#pragma once

struct Redirection
{
	int status;
	std::string url;
};

struct ErrorPage
{
	int status;
	std::string path;
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
	std::vector<std::string> port;
	std::vector<std::string> server_name;
	int client_max_body_size;
	std::vector<Location> location;
	std::vector<ErrorPage> error_page;
};

class ConfigFile
{
private:
	std::ifstream file;
	bool is_server;
	bool is_location;

	ConfigFile(void);
	ConfigFile(const ConfigFile &);
	ConfigFile &operator=(const ConfigFile &);

	void check_server(void);
	void check_location(void);
	void check_redirection(void);

	void set_server(void);
	void set_location(void);
	void set_redirection(void);
	void set_error_page(void);

	void duplicate_key(bool);

	// exceptions
	class Error : public std::exception
	{
	private:
		std::string msg;

	public:
		explicit Error(std::string _msg)
		{
			msg = "[ ERROR ] : " + _msg;
		}
		virtual const char *what() const throw()
		{
			return (msg.c_str());
		}
	};

public:
	~ConfigFile(void);
	ConfigFile(const char *);
	std::vector<Server> configuration;
};