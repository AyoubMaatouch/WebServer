#pragma once

#include "library.hpp"
#include <limits.h>

struct Redirection
{
	Redirection();
	Redirection &operator=(const Redirection &);

	int status;
	std::string url;
};

struct ErrorPage
{
	ErrorPage();
	ErrorPage &operator=(const ErrorPage &);

	int status;
	std::string path;
};

// struct Cgi
// {
// 	Cgi();
// 	Cgi &operator=(const Cgi &);
//
// 	std::string extension;
// 	std::string path;
// };

struct Location
{
	Location();
	Location(std::string s) : status(s) {};
	Location &operator=(const Location &);

	std::string path;
	std::string root;
	std::map<std::string, bool> allowed_method;
	std::vector<std::string> index;
	std::string upload;
	std::string status;
	std::map<std::string, std::string> cgi;
	bool auto_index;
	Redirection redirection;
	bool is_redirect;
};

struct Server
{
	Server();
	Server &operator=(const Server &);

	std::string host;
	std::vector<std::string> port;
	std::vector<std::string> server_name;
	long long client_max_body_size;
	std::vector<Location> location;
	std::vector<ErrorPage> error_page;
};

class ConfigFile
{
private:
	std::ifstream file;
	std::string previous_line;
	bool is_line;
	bool is_eof;

	ConfigFile(void);
	ConfigFile(const ConfigFile &);
	ConfigFile &operator=(const ConfigFile &);

	void check_all(void);
	void check_server(Server);
	void check_location(Location);
	void check_redirection(Redirection);

	void set_server(void);
	void set_location(void);
	void set_redirection(void);
	void set_error_page(void);
	void set_cgi(void);

	void check_for_error(bool, std::string);
	void duplicate_key(bool);
	void empty_value(bool);

	bool read_line(std::string &line);

public:
	~ConfigFile(void);
	ConfigFile(std::string);
	std::vector<Server> configuration;
};
