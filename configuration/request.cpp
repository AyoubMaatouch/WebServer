#include "library.hpp"

/*
**
* HEADER
**
*/

Header::Header(void)
	: method(),
	  path(),
	  status(),
	  version(),
	  host(),
	  port(),
	  connection(),
	  q_string(),
	  user_agent(),
	  accept(),
	  sec_gpc(),
	  sec_fetch_site(),
	  sec_fetch_mode(),
	  sec_fetch_dest(),
	  referer(),
	  accept_encoding(),
	  accept_language(),
	  transfer_encoding(),
	  content_type(),
	  content_length(),
	  location_id()
{
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
	return (*this);
}

/*
**
* DEFAULT
**
*/

Request::Request(void)
	: is_finished(false),
	  is_start_line(true),
	  is_header(true),
	  chunk(),
	  chunk_rest(),
	  chunk_length(),
	  is_chunk_length_read(false),
	  file_name(tmpname()),
	  body_length()
{
}

Request::~Request(void)
{
	std::remove(file_name.c_str());
}

Request::Request(const Request &copy)
{
	this->is_header = copy.is_header;
	this->is_start_line = copy.is_start_line;
	this->is_finished = copy.is_finished;
	this->chunk = copy.chunk;
	this->chunk_rest = copy.chunk_rest;
	this->chunk_length = copy.chunk_length;
	this->is_chunk_length_read = copy.is_chunk_length_read;
	this->header = copy.header;
	this->file_name = copy.file_name;
	this->body_length = copy.body_length;
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
	this->header = copy.header;
	this->file_name = copy.file_name;
	this->body_length = copy.body_length;

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
			header.content_length = atol(value.c_str());
		else if (key == "content-type")
			header.content_type = value;
	}

	is_header = false;
}

void Request::set_body(std::string body_req)
{
	if (body_req.size() == 0)
	{
		is_finished = true;
		return;
	}

	body_req = chunk_rest + body_req;
	chunk_rest.assign("");

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
					chunk.append(holder);
			}
			else
			{
				chunk_rest.append(body_req);
				body_req.assign("");
			}
		}
		else
		{
			chunk.append(body_req);
			body_req.assign("");
		}
		push_chunk();
	}
}

void Request::push_chunk(void)
{
	file.open(file_name, std::ios::app | std::ios::binary);

	if (header.transfer_encoding == "chunked")
	{
		if (chunk.size() == chunk_length)
		{
			file << chunk;
			chunk.assign("");
			chunk_length = 0;
			is_chunk_length_read = false;
		}
	}
	else
	{
		body_length += chunk.size();

		if (body_length == header.content_length)
			is_finished = true;

		file << chunk;
		chunk.assign("");
	}

	file.close();
}

/*
**
* CONTROLLER
**
*/

bool Request::isFinished(void)
{
	return (is_finished);
}

bool Request::header_finished(void)
{
	return !is_header;
}

void Request::check_request(Server &server)
{
	struct stat buf;

	header.location_id = 0;
	for (int i = 0; i < server.location.size(); i++)
	{
		if (server.location[i].path == header.path)
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
		header.status = "400";
	else if (header.path.size() > 2048)
		header.status = "414";
	else if (body_content.size() > server.client_max_body_size)
		header.status = "413";
	else if (stat((server.location[header.location_id].root).c_str(), &buf) < 0)
	{
		std::cout << "404 " << server.location[header.location_id].root << std::endl;
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

	file.close();
}

/*
**
* RELOAD
**
*/

void Request::reload(void)
{
	is_finished = false;
	is_start_line = true;
	is_header = true;
	chunk = "";
	chunk_rest = "";
	chunk_length = 0;
	is_chunk_length_read = false;
	std::remove(file_name.c_str());
	file_name = tmpname();
	body_length = 0;
	header.method = "";
	header.path = "";
	header.status = "";
	header.version = "";
	header.host = "";
	header.port = "";
	header.connection = "";
	header.user_agent = "";
	header.accept.clear();
	header.sec_gpc = "";
	header.sec_fetch_site = "";
	header.sec_fetch_mode = "";
	header.sec_fetch_dest = "";
	header.referer = "";
	header.location_id = 0;
	header.q_string = "";
	header.transfer_encoding = "";
	header.content_length = 0;
	header.content_type = "";
}