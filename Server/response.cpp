
#include "response.hpp"

Response::Response (Request req)
{
    Request req_obj = req;
    s_http = "HTTP/1.1 200 OK\n";
    s_content_type ="";
    s_content_length = "Content-Length: ";
    s_content = "";
    content_length = 0;
    if (req_obj.header.path == "./")
		{
			s_content_type = get_content_type("index.html") + "\n";
			std::ifstream file1("index.html");
			if (file1.is_open())
			{
				std::stringstream s;
				s << file1.rdbuf();
				s_content = s.str();
				int length = s_content.length();
				s_content_length += std::to_string(length);
				file1.close();
			}
		}
		else
		{
			s_content_type = get_content_type(req_obj.header.path) + "\n";
			// std::cout << "[s_content_type]: " << s_content_type << std::endl;
			std::ifstream file1(req_obj.header.path);

			if (file1.is_open())
			{
				std::stringstream s;
				s << file1.rdbuf();
				s_content = s.str();
				int length = s_content.length();
				s_content_length += std::to_string(length);
				file1.close();
			}
		}
}
std::string Response::get_response()
{
    std::string response = s_http + s_content_type + s_content_length + "\n\n" + s_content;


	return response;
}