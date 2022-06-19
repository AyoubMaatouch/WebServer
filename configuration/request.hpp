#pragma once

#include "library.hpp"

struct Server;

struct Header
{
	std::string method;
	std::string path;
	std::string status;
	std::string version;
	std::string host;
	std::string port;
	std::string connection;
	std::string q_string;
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
	std::string content_type;
	long content_length;
	int location_id;

	Header();
	Header &operator=(Header const &);
};

struct Request
{
private:
	std::ofstream file;
	std::string chunk;
	std::string chunk_rest;
	int body_length;
	int chunk_length;
	bool is_start_line;
	bool is_header;
	bool is_finished;
	bool is_chunk_length_read;
	void start_line(std::string);
	void set_header(std::string);
	void set_body(std::string);
	void push_chunk(void);

public:
	Request(void);
	Request(const Request &);
	Request &operator=(const Request &);
	~Request(void);
	Header header;
	std::string file_name;
	bool header_finished(void);
	bool isFinished(void);
	void set_request(std::string);
	void check_request(std::vector<Server> &);
	void reload(void);
};

#include "config_file.hpp"
