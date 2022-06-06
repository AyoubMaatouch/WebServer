#pragma once

#include "library.hpp"

struct Header
{
	std::string method;
	std::string path;
	std::string version;
	std::string host;
	std::string port;
	std::string connection;
	std::string user_agent;
	std::vector<std::string> accept;
	std::string sec_gpc;
	std::string sec_fetch_site;
	std::string sec_fetch_mode;
	std::string sec_fetch_dest;
	std::string referer;
	std::vector<std::string> accept_encoding;
	std::vector<std::string> accept_language;
	std::string transfer_encoding;
	int content_length;
};

struct Body
{
	std::ofstream file;
};

struct Request
{
private:
	// attributes
	bool is_start_line;
	bool is_header;
	bool is_body;
	bool is_finished;

	// chunck attributes
	std::string chunk;
	int chunk_length;
	bool is_chunk_length_read;
	bool is_chunk_read;

	// methods
	void start_line(std::string line);
	void set_header(std::string line);
	void set_body(std::string line);

public:
	// default
	Request();
	Request(std::string req);
	Request(const Request &copy);
	Request &operator=(const Request &copy);
	~Request();

	// check if the read is finished
	bool isFinished();

	// getter
	Header header;
	Body body;

	// setter
	void set_request(std::string req);
};