#include "library.hpp"

 void test_list(std::vector<std::string> list)
 {
 	for (size_t i = 0; i < list.size(); i++)
 	{
 		std::cout << list[i] << "-";
 	}
 }

 void error_pages(std::vector<ErrorPage> err)
 {
 	std::cout << "\n** ERROR PAGES " << " **\n\n";
 	for (size_t i = 0; i < err.size(); i++)
 	{
 		std::cout << "\t" << "[" << err[i].status << " | " << err[i].path << "]\n";

 	}
 }

  void cgi(std::vector<Cgi> _cgi)
 {
 	std::cout << "\n** CGI " << " **\n\n";
 	for (size_t i = 0; i < _cgi.size(); i++)
 	{
 		std::cout << "\t" << "[" << _cgi[i].extension << " | " << _cgi[i].path << "]\n";

 	}
 }


 void locations(std::vector<Location> locs)
 {
 	for (size_t i = 0; i < locs.size(); i++)
 	{
 		std::cout << "\n** LOCATION " << i << " **\n\n";
 		std::cout << "\t" << "path : [" << locs[i].path << "]\n";
 		std::cout << "\t" << "root : [" << locs[i].root << "]\n";
 		std::cout << "\t" << "upload : [" << locs[i].upload << "]\n";
 		std::cout << "\t" << "auto_index : [" << (locs[i].auto_index ? "on" : "off") << "]\n";
 		std::cout << "\t" << "index : ["; test_list(locs[i].index); std::cout << "]\n";
 		std::cout << "\t" << "redirection.url : [" << locs[i].redirection.url << "]\n";
 		std::cout << "\t" << "redirection.status : [" << locs[i].redirection.status << "]\n";
		cgi(locs[i].cgi);
 	}
 }


 int main()
 {
 	ConfigFile config_file("webserv.conf");

 	for (size_t i = 0; i < config_file.configuration.size(); i++)
 	{
 		std::cout << "\n** SERVER " << i << " **\n\n";
 		std::cout << "host : [" << config_file.configuration[i].host << "]\n";
 		std::cout << "client_max_body_size : [" << config_file.configuration[i].client_max_body_size << "]\n";
 		std::cout << "post : ["; test_list(config_file.configuration[i].port); std::cout << "]\n";
 		std::cout << "server_name : ["; test_list(config_file.configuration[i].server_name); std::cout << "]\n";

 		locations(config_file.configuration[i].location);
 		error_pages(config_file.configuration[i].error_page);
 	}
 	return 0;
 }