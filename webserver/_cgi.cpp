#include "response.hpp"


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
    if (pid == 0)
    {
        // child process
    // for POST request no need for close the pipe and for GET request need to close the pipe
        close(pipefd[0]);
        // if (req.header.method == "POST")
        //     {
        //         int fd = open(BODY_CONTENT_FILE, O_RDONLY);
        //         dup2(fd, 0);
        //     }
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 1);
        close(pipefd[1]);
        
        // setenv("CONTENT_LENGTH","",1);
        // setenv("CONTENT_TYPE","",1);
        // setenv("PATH_INFO",(char *const )script.c_str(),1);
        // setenv("REQUEST_METHOD",req.header.method.c_str(),1);
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
    std::cout << "CGI CONTENT :: " << std::endl << this->get_response() << std::endl;
}
