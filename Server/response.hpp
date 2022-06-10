#pragma once

#include "../config/request.hpp"
#include "helper_tools.hpp"

class Response {

    std::string s_http ;
    std::string s_status ;
	std::string s_content_type ;
	std::string s_content_length ;
	std::string s_content;
	int         content_length;
    std::map<std::string, std::string> map_status;
    
    public:
        Response();
        Response(Request req_obj);
        std::string getStatus(std::string const &code);
        std::string get_response();
        void response_error(Request &req);
        void set_map();
        ~Response(){
            // std::cout << "Response object destroyed" << std::endl;
        }

};