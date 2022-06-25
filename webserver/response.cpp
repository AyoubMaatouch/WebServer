#include "response.hpp"

#include <stdlib.h>
Response::Response() : s_cgi(""), len_send(0) 
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
	for (size_t i = 0; i < server.error_page.size(); i++)
	{
		if (req.header.status == to_string(server.error_page[i].status))
		{
			std::ifstream file2(server.error_page[i].path);
			if (file2.is_open())
			{
				s_content_type = get_content_type(server.error_page[i].path) + "\r\n";
				s_content.assign((std::istreambuf_iterator<char>(file2) ), (std::istreambuf_iterator<char>() ));
				s_content_length = std::to_string(s_content.length());
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

/***
 * GET REQUEST :
 *  Request to folder but with no "/" at the end, a "/" should be added and response should be set to code "301 moved permentely" 
 * 	if the request is a file ,  you should get the specific location for the request and replacing the req.header location with the current root
 * **/



void Response::get_method(Request &req, Server &server)
{


	std::string get_file = req.header.path;

	if (!replace_in_uri(get_file, _server_location.path, _server_location.root))
		throw std::runtime_error("replacing in uri failes!");
	if (isdir(get_file) && req.header.path[req.header.path.size() - 1] != '/')
		{
			s_status = map_status["301"];
			s_location = "Location: " + req.header.path  + "/" + "\r\n";
			s_content_type = "text/html\r\n";
			s_content_length = "6";
			s_content = "random";
			return ;
		}
	else if (isdir(get_file) && req.header.path[req.header.path.size() - 1] == '/')
		{
			if (_server_location.redirection.status == 301 || _server_location.redirection.status == 302 || _server_location.redirection.status == 307)
					s_location = std::string("Location: ") + _server_location.redirection.url + "\r\n";
			try 
			{
				std::ifstream file;
				
				// here you get the index from the current folder 
				// if not exist check for auto index else return 404 error page:

				get_file += get_index(get_file, _server_location.index); 
				s_content_type = get_content_type(get_file) + "\r\n";
				if (s_content_type.find("cgi") != std::string::npos )
					this->cgi_method(req, server, get_file);
				else
				{
					file.open(get_file);
					req.header.status = "200";
					s_content.assign((std::istreambuf_iterator<char>(file) ), (std::istreambuf_iterator<char>() ));
					s_content_length = to_string(s_content.length());
					file.close();
				}

			}
			catch (std::string &status)
			{
				if (status == "404" && _server_location.auto_index)
					{
						DIR *dir = opendir(get_file.c_str());
						open_directory(dir, req);
					}
				else
				{
					req.header.status = status;
					response_error(req, server);
				}
				return ;
			}
		}
		else
		{
			// here the request is a file so operate accordingly 
			try {

				if (get_per(get_file))
				{
					std::ifstream file;
					s_content_type = get_content_type(get_file) + "\r\n";
					if (s_content_type.find("cgi") != std::string::npos )
							this->cgi_method(req, server);
					else
					{
						file.open(get_file);
						req.header.status = "200";
						s_content.assign((std::istreambuf_iterator<char>(file) ), (std::istreambuf_iterator<char>() ));
						s_content_length = to_string(s_content.length());
						file.close();
					}
				}
			}
			catch (std::string &status)
			{
				req.header.status = status;
				response_error(req, server);
			}
		}
} 




/*

void Response::get_method(Request &req, Server &server)
{

	std::cout << "Req header path : " << req.header.path << std::endl;
	std::cout << "req header status : " << req.header.status << std::endl;
	std::cout << "server location path " << _server_location.path << std::endl;
	if (_server_location.redirection.status == 301 || _server_location.redirection.status == 302 || _server_location.redirection.status == 307)
	{
		s_location = std::string("Location: ") + _server_location.redirection.url + "\r\n";
	}

	if (req.header.path [req.header.path.size() - 1] == '/') // if path == /
	{
		std::ifstream file2;

		
		for (int i = 0; i < _server_location.index.size();i++) // Looping over config index
		{
			file2.open(_server_location.root + "/" + _server_location.index[i]);
			if (errno == EACCES)
			{
				req.header.status = "403";
				break;
			}
			else if (errno == ENOENT)
			{
				req.header.status = "404";
			}
			if (file2.is_open()) //If any index file opens
			{
				req.header.status = "200";
				s_content_type = get_content_type(_server_location.root + "/" + _server_location.index[i]) + "\r\n";
				if (s_content_type.find("cgi") != std::string::npos )
				{
					req.header.path += _server_location.index[i];
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
		std::ifstream file1(_server_location.root + req.header.path);
		if (file1.is_open()) // if we have permission to open the file
		{
			s_content_type = get_content_type(_server_location.root + req.header.path) + "\r\n";
			DIR *dir;
			std::cout << "s_content_type " << s_content_type << std::endl;
			std::cout << "trying to open " << _server_location.root + req.header.path << std::endl;
			if ((dir = opendir((_server_location.root + req.header.path).c_str()))) // If it's a Directory 
			{
				std::cout << "INSIDE DIRECTORY response.cpp" << std::endl;
				if_directory(req, dir, server);
			}
			else // if its a file
			{
				if (s_content_type.find("cgi") != std::string::npos )
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

*/


size_t Response::get_content_length()
{
	return (atoi(s_content_length.c_str()));
}



void Response::delete_method(Request &req, Server &server)
{
	
	std::ifstream file(_server_location.root + req.header.path);
	if (errno == EACCES)
		req.header.status = "403";
	else if (errno == ENOENT)
		req.header.status = "404";


	if (file.is_open())
	{
		if (isdir(_server_location.root + req.header.path))
			system(("rm -rf " + _server_location.root + req.header.path).c_str());
		else
			std::remove((_server_location.root + req.header.path).c_str());
		file.close();
		s_content_type = get_content_type("public/index.html") + "\r\n";
			//std::string s_style = "<style>*{transition: all 0.6s;}html {height: 100%;}body{font-family: \'Lato\', sans-serif;color: #888;margin: 0;}#main{display: table;width: 100%;height: 100vh;text-align: center;}fof{display: table-cell;vertical-align: middle;}.fof h1{font-size: 50px;display: inline-block;padding-right: 12px;animation: type .5s alternate infinite;}@keyframes type{from{box-shadow: inset -3px 0px 0px #888;}to{box-shadow: inset -3px 0px 0px transparent;}}</style>";

		s_content = "<html><head><link rel=\"stylesheet\" href=\"styles.css\"></head><body><div id=\"main\"><div class=\"fof\"><h1>DELETE request was succesful 204 </h1><h2>" + getStatus("204") + "</h2></div></div></body></html>" + "\r\n";

		s_content_length = std::to_string(s_content.length());
	}
	else
		response_error(req, server);
}
// ! file name get updated  when index is CALLED MULTIPLE TIMES

void Response::post_method(Request &req, Server &server)
{

	std::string text;
	
	std::string mybody;
	std::string get_file = req.header.path;


	if (!replace_in_uri(get_file, _server_location.path, _server_location.root))
		throw std::runtime_error("replacing in uri failes!");
	
		s_content_type = get_content_type(get_file) + "\r\n";
		// check a value in a string

		if (s_content_type.find("cgi") != std::string::npos )
		{
			this->cgi_method(req, server);
		}
		else
		{
			std::ifstream file(req.file_name);
			std::ofstream file1(_server_location.upload + "/uploaded_file", std::ios::binary);

			s_content_type = get_content_type("public/index.html") + "\r\n";
			if (file1.is_open() && file.is_open())
			{
			
				file1 << file.rdbuf();
				file1.close();
				file.close();
			}
			else 
			{
				s_content_type = get_content_type("public/index.html") + "\r\n";
				req.header.status = "403";
				response_error(req, server);
				return ;
			}
			std::string s_style = "<style>*{transition: all 0.6s;}html {height: 100%;}body{font-family: \'Lato\', sans-serif;color: #888;margin: 0;}#main{display: table;width: 100%;height: 100vh;text-align: center;}fof{display: table-cell;vertical-align: middle;}.fof h1{font-size: 50px;display: inline-block;padding-right: 12px;animation: type .5s alternate infinite;}@keyframes type{from{box-shadow: inset -3px 0px 0px #888;}to{box-shadow: inset -3px 0px 0px transparent;}}</style>";
			s_content = "<html><head><link rel=\"stylesheet\" href=\"styles.css\"></head><body><div id=\"main\"><div class=\"fof\"><h1>POST request was succesful " + req.header.status + "</h1><h2>" + getStatus(req.header.status) + "</h2></div></div></body></html>" + "\r\n";

			s_content_length = std::to_string(s_content.length());
		}
}

void Response::set_response (Request& req, Server &server, Location &server_location)
{

	set_map();
    s_http = "HTTP/1.1" ;
	s_status = map_status[req.header.status];
    s_content_length = "";
    s_content = "";
    content_length = 0;
	s_location = "";
	_server_location = server_location;
	 if (isdir(_server_location.root) && _server_location.root[_server_location.root.size() - 1] != '/')
	 		_server_location.root += "/";
	// ! Fix Redirection when response_error is triggered.
	if (_server_location.status == "404")
	{	
		req.header.status = "404";
		s_status = map_status["404"];
		response_error(req, server);
		return ;
	}
	
	if (req.header.status != "201" && req.header.status != "200" && req.header.status != "204")
		response_error(req, server);
	else if (req.header.method == "GET")
		get_method(req, server);
	else if (req.header.method == "POST")
		post_method(req, server);
	else if (req.header.method == "DELETE")
		delete_method(req, server);
}

std::string Response::get_response(Request &req, Server &server)
{
	// if (s_location != "")
	// 	s_status = map_status[to_string(_server_location.redirection.status)];
	(void)server;
	(void)req;
	if (!s_cgi.empty())
		return s_cgi;
	std::string response = "";
	response += s_http + s_status + "\r\n";
	if (!s_location.empty())
		response += s_location;
	if (!s_content_type.empty())
		response += "Content-type: " + s_content_type ;
	if (!s_content_length.empty())
		response +="Content-length: " + s_content_length  + "\r\n";
	if (!location.empty())
		response += location;
	if (!s_cookies.empty())
	{
		for (std::vector<std::string>::iterator it = s_cookies.begin() ; it != s_cookies.end(); it++)
			response += "Set-Cookie: " + (*it) + "\r\n";
	}
	response += "\r\n"; 
	if (!s_content.empty())
	 response += s_content ;
	return response;
}

void Response::open_directory(DIR *dir, Request& req_obj)
{
	std::vector<std::string> files;
	struct dirent *diread;

	while ((diread = readdir(dir)))
		files.push_back(diread->d_name);
	closedir(dir);
	s_content_type = get_content_type("public/index.html") + "\r\n";
	std::string style = "#auto{	text-align: center;	margin-top: 30px;}ul {	list-style-type: none;}p{	font-size: 20px;	font-weight: bold;	font-family: Verdana, Geneva, Tahoma, sans-serif;	text-decoration: none;}a:link { 	color: blue;}a:visited {	color: blue;}a:hover { 	color: blue;}a:active { 	color: blue;}";
	
	s_content = "<html><head><style>" + style + "</style> </head><body><h1 id=\"auto\">Index of " + req_obj.header.path + "</h1><ul>";

	for (size_t i = 0; i < files.size(); i++)
		s_content += "<li class=\"li\"><a href=\"" + req_obj.header.path + files[i] + "\"><p>" + files[i] + "</p></a></li>";
	s_content_length = to_string(s_content.length());
}

void Response::if_directory(Request &req, DIR *dir, Server &server)
{

	std::ifstream file2;
	struct stat *buf;

	(void)buf;
	for (size_t i = 0; i < _server_location.index.size() ; i++)
	{
		file2.open(_server_location.root + "/" + _server_location.index[i]);
		if (errno == EACCES)
		{
			req.header.status = "403";
			break;
		}
		else if (errno == ENOENT)
			req.header.status = "404";
		if (file2.is_open())
		{
			s_content_type = get_content_type(_server_location.root +  "/" + _server_location.index[i]) + "\r\n";
			if (s_content_type.find("cgi") != std::string::npos )
			{
				this->cgi_method(req, server);
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
	else if (_server_location.auto_index)
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