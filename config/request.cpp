#include "library.hpp"

/*
**
* DEFAULT
**
*/

Request::Request()
	: is_finished(false),
	  is_start_line(true),
	  is_header(true),
	  is_body(false),
	  chunk(),
	  chunk_length(),
	  is_chunk_length_read(false),
	  is_chunk_read(false)
{
	// remove the body_conent file if exist
	std::remove(BODY_CONTENT_FILE);
}

Request::Request(std::string req)
	: is_finished(false),
	  is_start_line(true),
	  is_header(true),
	  is_body(false),
	  chunk(),
	  chunk_length(),
	  is_chunk_length_read(false),
	  is_chunk_read(false)
{
	// remove the body_conent file if exist
	std::remove(BODY_CONTENT_FILE);

	// parse request
	set_request(req);
}

Request::~Request()
{ /* nothing */
}

Request::Request(const Request &copy)
{
	this->is_chunk_length_read = copy.is_chunk_length_read;
	this->is_chunk_read = copy.is_chunk_read;
	this->chunk_length = copy.chunk_length;
	this->chunk = copy.chunk;
	this->is_body = copy.is_body;
	this->is_header = copy.is_header;
	this->is_start_line = copy.is_start_line;
	this->is_finished = copy.is_finished;
	this->header = copy.header;
}

Request &Request::operator=(const Request &copy)
{
	this->is_chunk_length_read = copy.is_chunk_length_read;
	this->is_chunk_read = copy.is_chunk_read;
	this->chunk_length = copy.chunk_length;
	this->chunk = copy.chunk;
	this->is_body = copy.is_body;
	this->is_header = copy.is_header;
	this->is_start_line = copy.is_start_line;
	this->is_finished = copy.is_finished;
	this->header = copy.header;
	return *this;
}

/*
**
* SETTERS
**
*/

void Request::set_request(std::string req)
{
	std::stringstream ss(req);
	std::string line;

	while (std::getline(ss, line))
	{
		if (!is_body && line.size() == 0)
		{
			is_body = true;
			is_header = false;
		}
		else if (is_start_line)
			Request::start_line(line);
		else if (is_header)
		{
			Request::set_header(line);
		}
		else
		{
			Request::set_body(line);
		}
	}
}

void Request::start_line(std::string line)
{
	std::stringstream ss_line(line);
	ss_line >> header.method >> header.path >> header.version;
	header.path = "." + header.path;
	is_start_line = false;
}

void Request::set_header(std::string line)
{
	std::string key = line.substr(0, line.find(':'));
	std::string value = line.substr(line.find(':') + 2, line.size());

	if (key == "Host")
	{
		header.host = value.substr(0, value.find(':'));
		header.port = value.substr(value.find(':') + 1, value.size());
	}
	else if (key == "Connection")
		header.connection = value;
	else if (key == "User-Agent")
		header.user_agent = value;
	else if (key == "Sec-GPC")
		header.sec_gpc = value;
	else if (key == "Sec-Fetch-Site")
		header.sec_fetch_site = value;
	else if (key == "Sec-Fetch-Mode")
		header.sec_fetch_mode = value;
	else if (key == "Sec-Fetch-Dest")
		header.sec_fetch_dest = value;
	else if (key == "Referer")
		header.referer = value;
	else if (key == "Accept")
		list(value, &header.accept, ',');
	else if (key == "Accept-Encoding")
		list(value, &header.accept_encoding, ',');
	else if (key == "Accept-Language")
		list(value, &header.accept_language, ',');
	else if (key == "Transfer-Encoding")
		header.transfer_encoding = value;
	else if (key == "Content-Length")
		header.content_length = ft_atoi(value);
}

void Request::set_body(std::string line)
{
	// open the file
	body.file.open(BODY_CONTENT_FILE, std::ios_base::app);

	if (header.transfer_encoding == "chunked")
	{
		// check the length of a chunk is read
		if (!is_chunk_length_read)
		{
			chunk_length = hex_to_dec(line.substr(0, line.find('\r')));
			if (chunk_length == 0)
				is_finished = true;
			is_chunk_length_read = true;
		}
		else
		{
			chunk += line.substr(0, line.find('\r'));
			if (chunk.size() == chunk_length)
			{
				header.content_length -= chunk.size();
				body.file << chunk << "\n";
				is_chunk_read = true;
				is_chunk_length_read = false;
				chunk_length = 0;
				chunk = "";
			}
		}
	}
	else
	{
		if (line.size() == 0)
			is_finished = true;
		body.file << line << "\n";
	}

	// close the file
	body.file.close();
}

bool Request::isFinished()
{
	return (is_finished);
}

void Request::check_request(std::vector<Server *> &server) 
{
	struct stat buf;
	std::ifstream file(BODY_CONTENT_FILE);
	std::string body_content, text;

	while (file && getline (file, text)) 
		body_content += text;

	//! if header.status is empty = NO ERRORS
	if (header.transfer_encoding == "chunked" && header.content_length == 0)
		header.status = "400";
	else if (header.path.size() > 2048)
		header.status = "414";
	else if (body_content.size() > server[0]->client_max_body_size)
		header.status = "413";
	else if (stat(header.path.c_str(), &buf) < 0)
	{
		header.status = "404";
	}
	else if (header.method != "GET" && header.method != "POST" && header.method != "DELETE")
		header.status = "405";
	else
		header.status = "200";
}


Header &Header::operator=(Header const &copy)
{
	method = copy.method;
	path = copy.path ;
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

	return (*this);
}
