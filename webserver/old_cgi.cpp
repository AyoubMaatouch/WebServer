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
        }
        else
        {
            // req.body.file.open(BODY_CONTENT_FILE);
            // dup file to stdin
            int fd = open(BODY_CONTENT_FILE, O_RDONLY);
            if (fd < 0)
            {
                std::cout << "open file problem\n";
                exit(1);
            }
            // read from fd and set to query string
            char buf[1024];
            int n;
            while ((n = read(fd, buf, sizeof(buf))) > 0)
            {
                 query_string += std::string(buf, n);
            }

            // close(fd);
            dup2(fd, 0);
            dup2(pipefd[0], 0);
            close(pipefd[0]);
            dup2(pipefd[1], 1);
            close(pipefd[1]);
            close(pipefd[1]);
            
        }
        //     dup2(req.body.file.fileno(), 0);
        //     close(pipefd[1]);
        //     dup2(pipefd[0], 1);
        //     close(pipefd[0]);
        // }
            // dup2(pipefd[1], 1);

            // close(pipefd[1]);
            // dup2(pipefd[0], 0);
        // }
        // dup2(pipefd[1], 1);
        // close(pipefd[1]);
         if (req.header.method == "POST")
        {
            setenv("QUERY_STRING",query_string.c_str(),1); 
        }
         else
            setenv("QUERY_STRING",req.header.q_string.c_str(),1);
        // if (req.header.method == "POST")
        // {
            

            // setenv("CONTENT_LENGTH",to_string(req.body.body_length).c_str(), 1);
            // setenv("CONTENT_LENGTH", toreq.body.body_length.c_str(), 1);
        // }
            setenv("REQUEST_METHOD",req.header.method.c_str(), 1);
            setenv("CONTENT_TYPE","text/html",1);
            setenv("SCRIPT_FILENAME",script.c_str(),1);
            setenv("SCRIPT_NAME",req.header.path.c_str(),1);
            
            // setenv("SERVER_PORT",server.port[0].c_str(),1);
            // setenv("SERVER_PROTOCOL","HTTP/1.1",1);
            // setenv("SERVER_SOFTWARE","WebServer",1);
            // setenv("GATEWAY_INTERFACE","CGI/1.1",1);
            // setenv("REMOTE_ADDR",req.header.host.c_str(),1);
            // setenv("REMOTE_PORT",req.header.port.c_str(),1);
            // setenv("REQUEST_URI",req.header.path.c_str(),1);
            // setenv("REQUEST_PATH",req.header.path.c_str(),1);

        // execv inherits the environment variables of the parent process
        // std::cout << "QUERY_STRING " << query_string << std::endl;   
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
    std::cout << "CGI CONTENT ::  "<< req.header.path << std::endl << this->get_response() << std::endl;
}
