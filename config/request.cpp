#include "request.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

Request::Request() { /* nothing */ }

Request::~Request() { /* nothing */ }

// Request::Request(const Request &copy) { (void)copy; }

Request& Request::operator=(const Request &copy) { (void)copy; return *this; }

void list(std::string value, std::vector<std::string> *ptr)
{
	std::stringstream ss_value(value);
	while (std::getline(ss_value, value, ','))
	{
		std::remove(value.begin(), value.end(), ' ');
		ptr->push_back(value);
	}
}

void Request::set_header(std::string line)
{
	std::string key = line.substr(0, line.find(':'));
	std::string value = line.substr(line.find(':') + 2, line.size());
		
	if (key == "Host")
	{
		header.host = value.substr(0, value.find(':'));
		header.port = value.substr(value.find(':') + 1, value.size());
	}
	else if (key == "Connection") header.connection = value;
	else if (key == "User-Agent") header.user_agent = value;
	else if (key == "Sec-GPC") header.sec_gpc = value;
	else if (key == "Sec-Fetch-Site") header.sec_fetch_site = value;
	else if (key == "Sec-Fetch-Mode") header.sec_fetch_mode = value;
	else if (key == "Sec-Fetch-Dest") header.sec_fetch_dest = value;
	else if (key == "Referer") header.referer = value;
	else if (key == "Accept") list(value, &header.accept);
	else if (key == "Accept-Encoding") list(value, &header.accept_encoding);
	else if (key == "Accept-Language") list(value, &header.accept_language);
	
}

void Request::set_body(std::string line)
{
	std::string key = line.substr(0, line.find(':'));
	std::string value = line.substr(line.find(':') + 2, line.size());

	if (key == "Accept-Encoding") list(value, &body.accept_encoding);
	else if (key == "Accept-Language") list(value, &body.accept_language);
	else if (key == "Transfer-Encoding") body.transfer_encoding = value;
}

Request::Request(std::string req) : is_first(true)
{
	std::stringstream	ss(req);
	std::string			line;
	bool is_header;
	is_header = true;

	while (std::getline(ss, line))
	{
		if (line.size() == 0) is_header = false;
		else if (is_first) Request::first_line(line);
		else if (is_header) Request::set_header(line);
		else Request::set_body(line);
	}
}

void Request::first_line(std::string line)
{
	std::stringstream ss_line(line);
	ss_line >> header.method >> header.path >> header.version;
	header.path = "." + header.path;
	is_first = false;
}