#pragma once

#include <iostream>
#include <vector>

struct Header {
    std::string							method;
    std::string							path;
    std::string							version;
    std::string							host;
    std::string							port;
    std::string							connection;
    std::string							user_agent;
    std::vector<std::string>			accept;
    std::string							sec_gpc;
    std::string							sec_fetch_site;
    std::string							sec_fetch_mode;
    std::string							sec_fetch_dest;
    std::string							referer;
    std::vector<std::string>			accept_encoding;
    std::vector<std::string>			accept_language;
};

struct Body {
    std::string							transfer_encoding;
    std::vector<std::string>			accept_encoding;
    std::vector<std::string>			accept_language;
};

struct Request {
	private:
		// private
		Request();
		Request(const Request &copy);
		Request& operator=(const Request &copy);

		void set_header(std::string line);
		void set_body(std::string line);

		void first_line(std::string line);

		bool is_first;
	public:
		Request(std::string req);
		~Request();
		// getter
		Body							body;
		Header							header;
};