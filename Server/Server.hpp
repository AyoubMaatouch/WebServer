#pragma once

#include <iostream>


class _server
{
	public:
		_server(std::string host, int port) :  _host(host), _port(port) {};
		std::string get_host() { return _host; }

		int get_port() { return _port; }
		std::string get_path() { return _path; }
		~_server() {};
		int 		_socketfd;
	private:
		std::string _host;
		int 		_port;
		std::string _path;
};