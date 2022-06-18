#include "library.hpp"
#include <map>
#include <sys/errno.h>
	 /*
**
* DEFAULT
**
*/

int Request::unique_id = 0;

Request::Request(void)
	: is_finished(false),
	  is_start_line(true),
	  is_header(true),
	  chunk(),
	  chunk_rest(),
	  chunk_length(),
	  is_chunk_length_read(false),
	  _body_size(0)
{
	// body.body_length = 0;
	file_name = "body-content-" + intToString(++unique_id);
	header.file_name = file_name;
}

// Request::Request(std::string req)
	// // : is_finished(false),
	// //   is_start_line(true),
	// //   is_header(true),
	// //   chunk(),
	// //   chunk_rest(),
	// //   chunk_length(),
	// //   is_chunk_length_read(false)
// // {
	// body.body_length = 0;
	// file_name = "body-content-" + intToString(++unique_id);
	// header.file_name = file_name;
	// set_request(req);
// }

Request::~Request(void)
{
	// std::remove(file_name);
}

Request::Request(const Request &copy)
{
	this->is_header = copy.is_header;
	this->is_start_line = copy.is_start_line;
	this->is_finished = copy.is_finished;
	this->header_map = copy.header_map;
	this->chunk = copy.chunk;
	this->chunk_rest = copy.chunk_rest;
	this->chunk_length = copy.chunk_length;
	this->is_chunk_length_read = copy.is_chunk_length_read;
	this->header = copy.header;
}

Request &Request::operator=(const Request &copy)
{
	this->is_header = copy.is_header;
	this->is_start_line = copy.is_start_line;
	this->is_finished = copy.is_finished;
	this->chunk = copy.chunk;
	this->chunk_rest = copy.chunk_rest;
	this->chunk_length = copy.chunk_length;
	this->is_chunk_length_read = copy.is_chunk_length_read;
	this->header_map = copy.header_map;
	this->header = copy.header;
	this->file_name = copy.file_name;
	this->_body_size = copy._body_size;

	return *this;
}

/*
**
* SETTERS
**
*/

void Request::set_request(std::string req)
{
	if (is_header)
	{
		std::string header_req = req.substr(0, req.find("\r\n\r\n"));
		std::string body_req = req.substr(req.find("\r\n\r\n") + 4, req.size());

		set_header(header_req);
		set_body(body_req);
	}
	else
		set_body(req);
}

void Request::start_line(std::string line)
{
	std::stringstream ss_line(line);
	ss_line >> header.method >> header.path >> header.version;
	// header.path = "." + header.path;
	header.q_string = "";
	if (header.path.find("?") != std::string::npos)
	{
		header.q_string = header.path.substr(header.path.find("?") + 1, header.path.size());
		header.path = header.path.substr(0, header.path.find("?"));
	}

	is_start_line = false;
}

void Request::set_header(std::string header_req)
{
	std::stringstream ss_header(header_req);
	std::string line;

	while (std::getline(ss_header, line))
	{
		std::string key = lowerCase(line.substr(0, line.find(':')));
		std::string value = line.substr(line.find(':') + 2, line.size());

		value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());

		header_map.insert(std::pair<std::string, std::string>(key, value));

		if (is_start_line)
			start_line(line);
		else if (key == "host")
		{
			header.host = value.substr(0, value.find(':'));
			header.port = value.substr(value.find(':') + 1, value.size());
		}
		else if (key == "connection")
			header.connection = value;
		else if (key == "user-agent")
			header.user_agent = value;
		else if (key == "sec-gpc")
			header.sec_gpc = value;
		else if (key == "sec-fetch-site")
			header.sec_fetch_site = value;
		else if (key == "sec-fetch-mode")
			header.sec_fetch_mode = value;
		else if (key == "sec-fetch-dest")
			header.sec_fetch_dest = value;
		else if (key == "referer")
			header.referer = value;
		else if (key == "accept")
			list(value, &header.accept, ',');
		else if (key == "accept-encoding")
			list(value, &header.accept_encoding, ',');
		else if (key == "accept-language")
			list(value, &header.accept_language, ',');
		else if (key == "transfer-encoding")
			header.transfer_encoding = value;
		else if (key == "content-length")
		{
			header.content_length = stol(value);
			_body_size = header.content_length;
			// std::cout << "Content-Length is :" + value + " " + std::to_string(header.content_length) << std::endl;
		}
		else if (key == "content-type")
			header.content_type = value;
		// std::cout << "OUTSIDE: Content-Length: " << header.content_length << std::endl;
	}

	is_header = false;
}

bool Request::header_finished(void)
{
	return !is_header;
}

void Request::set_body(std::string body_req)
{
	if (body_req.size() == 0)
	{
		is_finished = true;
		return;
	}

	//open file or create new one
	// std::ofstream file;
	// file.open(file_name, std::ios::app);


	
	
	body_req = chunk_rest + body_req;
	chunk_rest = "";

	while (body_req.size())
	{
		if (header.transfer_encoding == "chunked")
		{
			if (body_req.find("\r\n") != std::string::npos && !isFinished())
			{
				std::string holder = body_req.substr(0, body_req.find("\r\n"));
				body_req = body_req.substr(body_req.find("\r\n") + 2, body_req.size());

				if (!is_chunk_length_read)
				{
					chunk_length = hex_to_dec(holder);

					if (chunk_length == 0)
						is_finished = true;

					is_chunk_length_read = true;
				}
				else
					chunk += holder;
			}
			else
			{
				chunk_rest.append(body_req);
				// chunk_rest += body_req;
				body_req = "";
			}
		}
		else
		{
			chunk += body_req;
			body_req = "";
		}
		push_chunk();
	}
	// is_finished = true;
	

	// std::cout << "BEFORE: Content-Length: " << header.header_map["content-length"] << std::endl;
	// int pid = fork();
	// if (pid == 0)
	// {
	// 	throw std::runtime_error("Body: " + std::to_string(body.body_length) + " HEADER: " + std::to_string(_body_size));
	// 	//  std::cout << "CHILD: Content-Length: " << header.content_length << std::endl;
	// }
	// else
	// {
	// 	// wait(NULL);
	// }
	// std::cout << "FILE Closed";
	// if (is_finished)
	// 	std::cout << " (Finished)";
	// else
	// 	std::cout << " (Not Finished)";
	// std::cout << std::endl;
}

void Request::push_chunk(void)
{
	body.file.open(header.file_name, std::ios::binary | std::ios::app);
	std::cout << strerror(errno) << std::endl;
	std::cout << "FILE Opened: "  << file_name << std::endl;
	if (header.transfer_encoding == "chunked")
	{
		if (chunk.size() == chunk_length)
		{
			body.file << chunk;
			chunk = "";
			chunk_length = 0;
			is_chunk_length_read = false;
		}
	}
	else
	{
		body.body_length += chunk.size();

		if (body.body_length == _body_size)
		{
			is_finished = true;
		}
		body.file << chunk;
		chunk = "";
	}
	body.file.close();
}

bool Request::isFinished(void)
{
	// if (is_finished)
	return (is_finished);
}

void Request::check_request(std::vector<Server> &server)
{
	struct stat buf;

	header.location_id = 0;
	for (int i = 0; i < server[0].location.size(); i++)
	{
		if (server[0].location[i].path == header.path)
		{
			header.location_id = i;
			break;
		}
	}
	std::ifstream file(file_name);
	std::string body_content, text;

	while (file && getline(file, text))
		body_content += text;

	std::string s = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?!#[]@$&'()*+,;=%";
	std::map<char, int> s_contain;
	for (int i = 0; i < s.length(); i++)
		s_contain[s[i]] = 1;

	for (int i = 0; i < header.path.length(); i++)
	{
		if (!s_contain[header.path[i]])
		{
			header.status = "400";
			return;
		}
	}

	if (header.transfer_encoding == "chunked" && header.content_length == 0)
		header.status = "400";
	else if (header.transfer_encoding != "chunked" && header.transfer_encoding != "")
		header.status = "501";
	else if ((header.method == "POST" && header.content_length == 0))
	{
		header.status = "400";
	}
	else if (header.path.size() > 2048)
		header.status = "414";
	else if (body_content.size() > server[0].client_max_body_size)
		header.status = "413";
	else if (stat((server[0].location[header.location_id].root + header.path).c_str(), &buf) < 0)
	{
		// std::cout << "ERRNO " << strerror(errno) << std::endl;
		std::cout << "ERROR 404 in request " << server[0].location[header.location_id].root + header.path << std::endl;
		header.status = "404";
	}
	else if (header.method != "GET" && header.method != "POST" && header.method != "DELETE")
		header.status = "405";
	else
	{
		if (header.method == "GET")
			header.status = "200";
		else if (header.method == "POST")
			header.status = "201";
		else if (header.method == "DELETE")
			header.status = "204";
	}
}

Header &Header::operator=(Header const &copy)
{
	method = copy.method;
	path = copy.path;
	status = copy.status;
	version = copy.version;
	host = copy.host;
	port = copy.port;
	connection = copy.connection;
	user_agent = copy.user_agent;
	accept = copy.accept;
	sec_gpc = copy.sec_gpc;
	sec_fetch_site = copy.sec_fetch_site;
	sec_fetch_mode = copy.sec_fetch_mode;
	sec_fetch_dest = copy.sec_fetch_dest;
	referer = copy.referer;
	location_id = copy.location_id;
	q_string = copy.q_string;
	transfer_encoding = copy.transfer_encoding;
	content_length = copy.content_length;
	content_type = copy.content_type;
	file_name = copy.file_name;
	return (*this);
}

/*
**
* TEST
**
*/

void each(std::vector<std::string> table)
{
	for (size_t i = 0; i < table.size(); i++)
	{
		std::cout << table[i] << "-";
	}
}

void Request::test_output(void)
{
	std::cout << "method : [" << header.method << "]\n";
	std::cout << "path : [" << header.path << "]\n";
	std::cout << "status : [" << header.status << "]\n";
	std::cout << "version : [" << header.version << "]\n";
	std::cout << "host : [" << header.host << "]\n";
	std::cout << "port : [" << header.port << "]\n";
	std::cout << "connection : [" << header.connection << "]\n";
	std::cout << "user_agent : [" << header.user_agent << "]\n";
	std::cout << "sec_gpc : [" << header.sec_gpc << "]\n";
	std::cout << "sec_fetch_site : [" << header.sec_fetch_site << "]\n";
	std::cout << "sec_fetch_mode : [" << header.sec_fetch_mode << "]\n";
	std::cout << "sec_fetch_dest : [" << header.sec_fetch_dest << "]\n";
	std::cout << "referer : [" << header.referer << "]\n";
	std::cout << "transfer_encoding : [" << header.transfer_encoding << "]\n";
	std::cout << "content_length : [" << header.content_length << "]\n";
	std::cout << "accept : [";
	each(header.accept);
	std::cout << "]\n";
	std::cout << "accept_encoding : [";
	each(header.accept_encoding);
	std::cout << "]\n";
	std::cout << "accept_language : [";
	each(header.accept_language);
	std::cout << "]\n";
}