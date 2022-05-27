#pragma once

#include "../config/request.hpp"
#include "helper_tools.hpp"

class Response {

    std::string s_http ;
	std::string s_content_type ;
	std::string s_content_length ;
	std::string s_content;
	int         content_length;
    
    public:
        Response(Request req_obj);
        std::string get_response();
        ~Response(){
            // std::cout << "Response object destroyed" << std::endl;
        }

};