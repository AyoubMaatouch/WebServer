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
	long content_length;
	std::string content_type;
	int location_id;
	std::map<std::string, std::string> header_map;
	Header &operator=(Header const &);
};

struct Body
{
	std::ofstream file;
	int body_length;
	Body(): body_length(0) {}
};

struct Request
{
private:
	// attributes
	bool is_start_line;
	bool is_header;
	bool is_finished;
	static int unique_id;
	std::string file_name;

	// chunck attributes
	std::string chunk;
	std::string chunk_rest;
	int chunk_length;
	bool is_chunk_length_read;

	// methods
	void start_line(std::string);
	void set_header(std::string);
	void set_body(std::string);
	void push_chunk(void);

public:
	// default
	bool header_finished(void);
	Request(void);
	Request(std::string);
	Request(const Request &);
	Request &operator=(const Request &);
	~Request(void);

	// check if the read is finished
	bool isFinished(void);

	// getter
	Header header;
	long _body_size;
	Body body;
	std::map<std::string, std::string> header_map;

	// setter
	void set_request(std::string);
	void check_request(std::vector<Server> &);

	// test
	void test_output(void);
};

#include "config_file.hpp"
