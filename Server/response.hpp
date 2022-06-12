#pragma once

#include "../config/request.hpp"
#include "helper_tools.hpp"
#include <dirent.h>

class Response {

    std::string s_http ;
    std::string s_status ;
	std::string s_content_type ;
	std::string s_content_length ;
	std::string s_content;
	int         content_length;
    std::map<std::string, std::string> map_status;
    
    public:
        size_t len_send;
        size_t get_content_length();
        Response();
        Response(Request req_obj, std::vector<Server> &server);
        void set_response(Request req_obj, std::vector<Server> &server);
        std::string getStatus(std::string const &code);
        std::string get_response();
        void response_error(Request &req);
        void open_directory(DIR *dir, Request req_obj);
        void get_method(Request &req, std::vector<Server> &server);
        void if_directory(Request &req, DIR *dir, std::vector<Server> &server);
        void cgi_method(Request &req, std::vector<Server> &server);
        void set_map();
        // std::string to_string(int i);
        ~Response(){
            // std::cout << "Response object destroyed" << std::endl;
        }

};