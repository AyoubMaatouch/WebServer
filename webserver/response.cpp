#include "response.hpp"

#include <stdlib.h>
Response::Response() : len_send(0)
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
	map_status["307"] = " 307 Temporary Redirect";
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
	map_status["501"] = " 501 Not Implemented";
    map_status["502"] = " 502 Bad Gateway";
    map_status["503"] = " 503 Service Unavailable";
    map_status["504"] = " 504 Gateway Timeout";
    map_status["505"] = " 505 HTTP Version Not Supported";
}

std::string Response::getStatus(std::string const &code)
{
	return (map_status[code]);
}

void Response::response_error(Request &req, Server &server)
{
	for (int i = 0; i < server.error_page.size(); i++)
	{
		if (req.header.status == to_string(server.error_page[i].status))
		{
			std::ifstream file2(server.error_page[i].path);
			if (file2.is_open())
			{
				s_content_type = get_content_type(server.error_page[i].path) + "\r\n";
				s_content.assign((std::istreambuf_iterator<char>(file2) ), (std::istreambuf_iterator<char>() ));
				s_content_length = std::to_string(s_content.length());
				std::cout << "ERROR PAGE PATH " << server.error_page[i].path << std::endl;
				return;
			}
			else
				break ;
		}
	}

	s_content_type = get_content_type("public/index.html") + "\r\n";
	std::string s_style = "<style>*{transition: all 0.6s;}html {height: 100%;}body{font-family: \'Lato\', sans-serif;color: #888;margin: 0;}#main{display: table;width: 100%;height: 100vh;text-align: center;}fof{display: table-cell;vertical-align: middle;}.fof h1{font-size: 50px;display: inline-block;padding-right: 12px;animation: type .5s alternate infinite;}@keyframes type{from{box-shadow: inset -3px 0px 0px #888;}to{box-shadow: inset -3px 0px 0px transparent;}}</style>";

	s_content = "<html><head><link rel=\"stylesheet\" href=\"styles.css\"></head><body><div id=\"main\"><div class=\"fof\"><h1>Error " + req.header.status + "</h1><h2>" + getStatus(req.header.status) + "</h2><img src=\"finawa.gif\" loop=infinite></div></div></body></html>" + "\r\n";

	s_content_length = std::to_string(s_content.length());
}

void Response::get_method(Request &req, Server &server)
{


	if (server.location[req.header.location_id].redirection.status == 301 || server.location[req.header.location_id].redirection.status == 302 || server.location[req.header.location_id].redirection.status == 307)
	{
		s_location = std::string("Location: ") + server.location[req.header.location_id].redirection.url + "\r\n";
	}


	if (req.header.path == "/") // if path == /
	{
		std::ifstream file2;
		
		for (int i = 0; i < server.location[req.header.location_id].index.size();i++) // Looping over config index
		{
			file2.open(server.location[req.header.location_id].root + "/" + server.location[req.header.location_id].index[i]);
			if (errno == EACCES)
			{
				req.header.status = "403";
				break;
			}
			else if (errno == ENOENT)
				req.header.status = "404";
			if (file2.is_open()) //If any index file opens
			{
				req.header.status = "200";
				s_content_type = get_content_type(server.location[req.header.location_id].root + "/" + server.location[req.header.location_id].index[i]) + "\r\n";
				if (s_content_type == "application/octet-stream\r\n")
				{
					req.header.path += server.location[req.header.location_id].index[i];
					this->cgi_method(req, server);
					
				}
				else
				{
					s_content.assign((std::istreambuf_iterator<char>(file2) ), (std::istreambuf_iterator<char>() ));
					s_content_length = to_string(s_content.length());
				}
				break ;
			}
		}
		if (file2.is_open() && req.header.status == "200")
			file2.close();
		else // no files from index found header set by errno
		{
			file2.close();
			response_error(req, server);
		}
	}
	else //If path isnt "/"
	{
		std::ifstream file1(server.location[req.header.location_id].root + req.header.path);
		if (file1.is_open()) // if we have permission to open the file
		{
			s_content_type = get_content_type(req.header.path) + "\r\n";
			DIR *dir;
			std::cout << "PATH ISNT / AND OPENED " << req.header.path << std::endl;

			if ((dir = opendir((server.location[req.header.location_id].root  + req.header.path).c_str()))) // If it's a Directory 
			{
				std::cout << "INSIDE DIRECTORY response.cpp" << std::endl;
				if_directory(req, dir, server);
			}
			else // if its a file
			{
				if (s_content_type == "application/octet-stream\r\n")
				{
					this->cgi_method(req, server);
					std::cout << "CGI" << std::endl;
				}
				else
				{
					s_content.assign((std::istreambuf_iterator<char>(file1) ), (std::istreambuf_iterator<char>() ));
					s_content_length = to_string(s_content.length());
					file1.close();
				}
			}
		}
		else //Permission error
		{
			file1.close();
			req.header.status = "403";
			response_error(req, server);
		}
	}

}
size_t Response::get_content_length()
{
	return (atoi(s_content_length.c_str()));
}

Response::Response (Request &req, Server &server)
{
	set_map();
    s_http = "HTTP/1.1" ;
	s_status = map_status[req.header.status];
    s_content_length = "";
    s_content = "";
    content_length = 0;
	
	std::cout << "MADE IT HERE\n" << std::endl;
	if (req.header.status != "201" && req.header.status != "200")
		response_error(req, server);
	else if (req.header.method == "GET") //! Try autoindex on test in the public file...
		get_method(req, server);
	else if (req.header.method == "POST")
		post_method(req, server);
	else if (req.header.method == "DELETE")
		;
}


// ! file name get updated  when index is CALLED MULTIPLE TIMES

void Response::post_method(Request &req, Server &server)
{

	std::string text;
	
	std::string mybody;
	DIR* dir;
	
	std::cout << server.location[req.header.location_id].root  + req.header.path << std::endl;
	// if ((dir = opendir((server.location[req.header.location_id].root  + req.header.path).c_str()))) // If it's a Directory 
	// 	if_directory(req, dir, server);
	// else // if its a file
	// {
		// if (req.header.path == "/")
		//   req.header.path += "upload.php";	
		s_content_type = get_content_type(req.header.path) + "\r\n";
		std::cout << "POST content type " << s_content_type << std::endl;
		if (s_content_type == "application/octet-stream\r\n")
		{
			std::cout << "MADE IT TO octet stream" << std::endl;
			this->cgi_method(req, server);
			std::cout << "POST CGI" << std::endl;
		}
	
			// std::ifstream file(req.header.file_name);
			// std::ofstream file1(server.location[req.header.location_id].upload + "/uploaded_file", std::ios::binary);


			// if (file1.is_open() && file.is_open())
			// {
			// 	std::cout << "FILE OPENED" << std::endl;
			
			// 	file1 << file.rdbuf();
			// 	file1.close();
			// 	file.close();
			// }
			// else 
			// {
			// 	s_content_type = get_content_type("public/index.html") + "\r\n";
			// 	req.header.status = "403";
			// 	response_error(req, server);
			// 	return ;
			// }

			// s_content_type = get_content_type("public/index.html") + "\r\n";
			// std::string s_style = "<style>*{transition: all 0.6s;}html {height: 100%;}body{font-family: \'Lato\', sans-serif;color: #888;margin: 0;}#main{display: table;width: 100%;height: 100vh;text-align: center;}fof{display: table-cell;vertical-align: middle;}.fof h1{font-size: 50px;display: inline-block;padding-right: 12px;animation: type .5s alternate infinite;}@keyframes type{from{box-shadow: inset -3px 0px 0px #888;}to{box-shadow: inset -3px 0px 0px transparent;}}</style>";

			// s_content = "<html><head><link rel=\"stylesheet\" href=\"styles.css\"></head><body><div id=\"main\"><div class=\"fof\"><h1>POST request was succesful " + req.header.status + "</h1><h2>" + getStatus(req.header.status) + "</h2></div></div></body></html>" + "\r\n";

			// s_content_length = std::to_string(s_content.length());
}

void Response::set_response (Request& req, Server &server)
{
	set_map();
    s_http = "HTTP/1.1" ;
	s_status = map_status[req.header.status];
    s_content_length = "";
    s_content = "";
    content_length = 0;
	s_location = "";
	
	if (req.header.status != "201" && req.header.status != "200")
		response_error(req, server);
	else if (req.header.method == "GET")
		get_method(req, server);
	else if (req.header.method == "POST")
		post_method(req, server);
	else if (req.header.method == "DELETE")
		;
}

std::string Response::get_response(Request &req, Server &server)
{
	if (s_location != "")
		s_status = map_status[to_string(server.location[req.header.location_id].redirection.status)];

	std::string response = s_http + s_status + "\r\n"  + "Content-type: " + s_content_type + location + s_c_location +"Content-length: " + s_content_length + "\r\n\r\n" + s_content ;
	return response;
	// return cgi_content;
}

void Response::open_directory(DIR *dir, Request& req_obj)
{
	std::vector<std::string> files;
	struct dirent *diread;

	while ((diread = readdir(dir)))
		files.push_back(diread->d_name);
	closedir(dir);
	s_content_type = get_content_type("public/index.html") + "\r\n";
	s_content = "<html><head><link rel=\"stylesheet\" href=\"autoindex.css\"></head><body><h1 id=\"auto\">Index of " + req_obj.header.path + "</h1><ul>";

	for (int i = 0; i < files.size(); i++)
		s_content += "<li class=\"li\"><a href=\"" + req_obj.header.path + "/" + files[i] + "\"><p>" + files[i] + "</p></a></li>";
	s_content_length = to_string(s_content.length());
}

void Response::if_directory(Request &req, DIR *dir, Server &server)
{

	std::ifstream file2;
	struct stat *buf;

	for (int i = 0; i < server.location[req.header.location_id].index.size() ; i++)
	{
		std::cout << "DIRECTORY " << server.location[req.header.location_id].root + "/" + req.header.path + "/" + server.location[req.header.location_id].index[i] << std::endl;
		file2.open(server.location[req.header.location_id].root + "/" + req.header.path + "/" + server.location[req.header.location_id].index[i]);
		if (errno == EACCES)
		{
			req.header.status = "403";
			break;
		}
		else if (errno == ENOENT)
			req.header.status = "404";
		if (file2.is_open())
		{
			s_content_type = get_content_type(server.location[req.header.location_id].root +  "/" + server.location[req.header.location_id].index[i]) + "\r\n";
			if (s_content_type == "application/octet-stream\r\n")
				{
					this->cgi_method(req, server);
					std::cout << "CGI" << std::endl;
				}
			else
			{
				s_content.assign((std::istreambuf_iterator<char>(file2) ), (std::istreambuf_iterator<char>() ));
				s_content_length = to_string(s_content.length());
				req.header.status = "200";
			}
			break ;
		}

	}

	if (file2.is_open())
		file2.close();
	else if (server.location[req.header.location_id].auto_index)
		open_directory(dir, req);
	else
	{
		file2.close();
		response_error(req, server);
	}

}

Response::~Response()
{
        
}