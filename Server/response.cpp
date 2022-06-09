#include "response.hpp"


Response::Response()
{


}



void Response::set_map()
{
	map_status["200"] = " 200 OK";
    map_status["201"] = " 201 Created";
    map_status["202"] = " 202 Accepted";
    map_status["204"] = " 204 No Content";
    map_status["206"] = " 206 Partial Content";
    map_status["301"] = " 301 Moved Permanently";
    map_status["302"] = " 302 Found";
    map_status["304"] = " 304 Not Modified";
    map_status["400"] = " 400 Bad Request";
    map_status["401"] = " 401 Unauthorized";
    map_status["403"] = " 403 Forbidden";
    map_status["404"] = " 404 Not Found";
    map_status["405"] = " 405 Method Not Allowed";
    map_status["406"] = " 406 Not Acceptable";
    map_status["408"] = " 408 Request Timeout";
    map_status["411"] = " 411 Length Required";
    map_status["413"] = " 413 Payload Too Large";
    map_status["414"] = " 414 URI Too Long";
    map_status["415"] = " 415 Unsupported Media Type";
    map_status["429"] = " 429 Too Many Requests";
    map_status["431"] = " 431 Request Header Fields Too Large";
    map_status["500"] = " 500 Internal Server Error";
	map_status["501"] = " Not Implemented";
    map_status["502"] = " 502 Bad Gateway";
    map_status["503"] = " 503 Service Unavailable";
    map_status["504"] = " 504 Gateway Timeout";
    map_status["505"] = " 505 HTTP Version Not Supported";
}

std::string Response::getStatus(std::string const &code)
{
	return (map_status[code]);
}

Response::Response (Request req)
{
	set_map();
    Request req_obj = req;
    s_http = "HTTP/1.1" ;
	s_status = map_status[req.header.status];
    //s_content_type = "";
    s_content_length = "Content-Length: ";
    s_content = "";
    content_length = 0;
	std::cout << "Header " + req.header.status << "Path: " << req.header.path << std::endl;
	if (req.header.status != "201" && req.header.status != "200")
	{
		s_content_type = get_content_type("public/index.html") + "\n";
		s_content = "<html><head><link rel=\"stylesheet\" href=\"public/styles.css\"></head><body><div id=\"main\"><div class=\"fof\"><h1>Error " + req.header.status + "</h1><h2>" + s_status + "</h2></div></div></body></html>" + "\n";

		s_content_length = std::to_string(s_content.length());
	}
	else if (req_obj.header.path == "./")
	{
		s_content_type = get_content_type("public/index.html") + "\n";
		std::ifstream file1("public/index.html");
		if (file1.is_open())
		{
			std::stringstream s;
			s << file1.rdbuf();
			s_content = s.str();
			s_content_length += std::to_string(s_content.length());
			file1.close();
		}
	}		
	else
	{
		std::cout << "Made it here css\n";
		//exit(0);
		s_content_type = get_content_type(req_obj.header.path) + "\n";
		// std::cout << "[s_content_type]: " << s_content_type << std::endl;
		std::ifstream file1(req_obj.header.path);
		if (file1.is_open())
		{
			std::stringstream s;
			s << file1.rdbuf();
			s_content = s.str();
			s_content_length += std::to_string(s_content.length());
			file1.close();
		}
	}
}
std::string Response::get_response()
{
	//std::string response = "HTTP/1.1 200 OK\nDate: Thu, 09 Dec 2004 12:07:48 GMT\nServer: IBM_CICS_Transaction_Server/3.1.0(zOS)\nContent-type: text/plain\nContent-length: 0\n\n";
	std::string response = s_http + s_status + "\n" + "Content-type: " + s_content_type + "Content-length: " + s_content_length + "\n\n" + s_content;

	return response;
}