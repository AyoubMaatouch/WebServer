#include "request.hpp"
#include <sstream>


int main() {
	Request req("GET /favicon.ico HTTP/1.1\nHost: localhost:8080\nConnection: keep-alive\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.67 Safari/537.36\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\nSec-GPC: 1\nSec-Fetch-Site: same-origin\nSec-Fetch-Mode: no-cors\nSec-Fetch-Dest: image\nReferer: http://localhost:8080/\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9\n\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-US,en;q=0.9");
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<< HEADER >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	std::cout << "method : [" << req.header.method  << "]" << "\n";
	std::cout << "path : [" << req.header.path  << "]" << "\n";
	std::cout << "version : [" << req.header.version  << "]" << "\n";
	std::cout << "host : [" << req.header.host  << "]" << "\n";
	std::cout << "port : [" << req.header.port  << "]" << "\n";
	std::cout << "connection : [" << req.header.connection  << "]" << "\n";
	std::cout << "user_agent : [" << req.header.user_agent  << "]" << "\n";
	std::cout << "accept : [";
	for (size_t i = 0; i < req.header.accept.size(); i++)
	{
		std::cout << req.header.accept[i] << " | ";
	}
	std::cout << "]" << "\n";
	std::cout << "sec_gpc : [" << req.header.sec_gpc  << "]" << "\n";
	std::cout << "sec_fetch_site : [" << req.header.sec_fetch_site  << "]" << "\n";
	std::cout << "sec_fetch_mode : [" << req.header.sec_fetch_mode  << "]" << "\n";
	std::cout << "sec_fetch_dest : [" << req.header.sec_fetch_dest  << "]" << "\n";
	std::cout << "referer : [" << req.header.referer  << "]" << "\n";
	std::cout << "accept_encoding : [";
	for (size_t i = 0; i < req.header.accept_encoding.size(); i++)
	{
		std::cout << req.header.accept_encoding[i] << " | ";
	}
	std::cout << "]" << "\n";
	std::cout << "accept_language : [";
	for (size_t i = 0; i < req.header.accept_language.size(); i++)
	{
		std::cout << req.header.accept_language[i] << " | ";
	}
	std::cout << "]" << "\n";
	std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<< BODY >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n";
	std::cout << "accept_encoding : [";
	for (size_t i = 0; i < req.body.accept_encoding.size(); i++)
	{
		std::cout << req.body.accept_encoding[i] << " | ";
	}
	std::cout << "]" << "\n";
	std::cout << "accept_language : [";
	for (size_t i = 0; i < req.body.accept_language.size(); i++)
	{
		std::cout << req.body.accept_language[i] << " | ";
	}
	std::cout << "]" << "\n";
    return 0;
}
