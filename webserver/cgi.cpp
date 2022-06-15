#include "response.hpp"
#include <fcntl.h>

void Response::cgi_method(Request &req, Server &server)
{
// pipe to communicate with the cgi program

    int pipefd[2];
    pipe(pipefd);
    std::string s_cgi_content("");

// fork the cgi program
//    std::string file = "/Users/aymaatou/Desktop/WebServer/server/public/indexo.py";
    std::string script = server.location[req.header.location_id].root + req.header.path;
    char *const parm[] = {(char *const )server.location[req.header.location_id].cgi.c_str(), (char *const )script.c_str(), NULL};
//    char *const parm[] = {"/usr/bin/env",NULL};
    pid_t pid = fork();
    std::string query_string = "";
    if (pid == 0)
    {
        // child process
    // for POST request no need for close the pipe and for GET request need to close the pipe
        if (req.header.method == "GET")
        {
            close(pipefd[0]);
            dup2(pipefd[1], 1);
            close(pipefd[1]);
            
            if (req.header.q_string != "")
            {
                setenv("QUERY_STRING",req.header.q_string.c_str(),1);             
            }
        }
        else
        {
            
            // dup file to stdin
            int fd = open(BODY_CONTENT_FILE, O_RDONLY);
            if (fd < 0)
            {
                std::cout << "emallah  file not found\n";
                throw std::runtime_error("505 Internal Server Error");
            }
            // read from fd and set to query string
            char buffer[1024];
            int n;
            while ((n = read(fd, buffer, 1024)) > 0)
            {
                query_string += std::string(buffer, n);
            }
            setenv("QUERY_STRING",query_string.c_str(),1);             
            setenv("CONTENT_TYPE","text/html",1);
            setenv("CONTENT_LENGTH",std::to_string(query_string.size()).c_str(),1);

            // close(fd);
            dup2(fd, 0);
            close(pipefd[0]);
            dup2(pipefd[1], 1);
            close(pipefd[1]);
        }
        setenv("REQUEST_METHOD",req.header.method.c_str(), 1);
        setenv("SCRIPT_FILENAME",script.c_str(),1);
        setenv("SCRIPT_NAME",req.header.path.c_str(),1);
        
        setenv("SERVER_PORT",server.port[0].c_str(),1);
        setenv("SERVER_PROTOCOL","HTTP/1.1",1);
        setenv("SERVER_SOFTWARE","WebServer",1);
        setenv("GATEWAY_INTERFACE","CGI/1.1",1);
        setenv("REMOTE_ADDR",req.header.host.c_str(),1);
        setenv("REMOTE_PORT",req.header.port.c_str(),1);
        setenv("REQUEST_URI",req.header.path.c_str(),1);
        setenv("REQUEST_PATH",req.header.path.c_str(),1);

        // execv inherits the environment variables of the parent process

        execv((char *const )server.location[req.header.location_id].cgi.c_str(), parm);
        exit(0);
    }   
    else
    {
        // parent process
        close(pipefd[1]);
        char buf[1024];
        int n;
        while ((n = read(pipefd[0], buf, 1024)) > 0)
        {
            s_cgi_content.append(buf, n);
        }
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }
    
    std::string line;
    std::stringstream ss(s_cgi_content);

    while (std::getline(ss, line))
    {
        if (line.find("X-Powered-By:") != std::string::npos)
            continue;
        if (line.find("Status:") != std::string::npos)
        {
            std::string status = line.substr(line.find(":") + 1);
            s_status = status;
        }
        else if (line.find("Content-type:") != std::string::npos)
        {
            std::string content_type = line.substr(line.find(":") + 1);
            s_content_type = content_type;
        }
        else
            s_content.append(line);
    }
    s_content_length = to_string(s_content.length());
    std::cout << "CGI CONTENT ::  "<< req.header.path << std::endl << this->get_response(req, server) << std::endl;
}
