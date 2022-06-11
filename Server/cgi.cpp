#include "response.hpp"
#include <unistd.h>
void Response::cgi_method(Request &req, std::vector<Server *> &server)
{
    std::cout << "request path "<< req.header.path << std::endl;
    std::cout << "request method "<< req.header.method << std::endl;
    std::cout << "CGI PATH "<< server[0]->location[0]->cgi  << std::endl;
    std::string file = "/Users/aymaatou/Desktop/WebServer/server/index.php";
    char *const parm[] = {(char *const )server[0]->location[0]->cgi.c_str(), (char *const )file.c_str(), NULL};
    int ret = execv((const char*)server[0]->location[0]->cgi.c_str(), parm);
    std::cout << "CGI method executed: "<< ret << std::endl;


}