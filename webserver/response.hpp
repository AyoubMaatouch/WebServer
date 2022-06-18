#pragma once

#include "../configuration/request.hpp"
#include "helper_tools.hpp"
#include <dirent.h>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

class Response {

    std::string s_http ;
    std::string s_status ;
	std::string s_content_type ;
	std::string s_content_length ;
	std::string s_content;
	int         content_length;
    std::string s_location;
    std::string s_c_location;
    std::map<std::string, std::string> map_status;

    std::string location;
    
    
    public:
        size_t len_send;
        size_t get_content_length();
        Response();
        ~Response();
        Response(Request &req_obj, Server &server);
        void set_response(Request &req_obj, Server &server);
        std::string getStatus(std::string const &code);
        std::string get_response(Request &req, Server &server);
        void response_error(Request &req, Server &server);
        void open_directory(DIR *dir, Request &req_obj);
        void get_method(Request &req, Server &server);
        void post_method(Request &req, Server &server);
        void if_directory(Request &req, DIR *dir, Server &server);
        void cgi_method(Request &req,  Server &server);
        bool get_cgi();
        void set_map();

};