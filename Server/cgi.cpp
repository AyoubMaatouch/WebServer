#include "response.hpp"


void Response::cgi_method(Request &req, Location &location)
{
// pipe to communicate with the cgi program

    int pipefd[2];
    pipe(pipefd);
     std::string s_cgi_content("");

// fork the cgi program
   std::string file = "/Users/aymaatou/Desktop/WebServer/server/public/index2.php";
   char *const parm[] = {(char *const )location.cgi.c_str(), (char *const )file.c_str(), NULL};
//    char *const parm[] = {"/usr/bin/env",NULL};
    pid_t pid = fork();
    if (pid == 0)
    {
        // child process
    // for POST request no need for close the pipe and for GET request need to close the pipe
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        close(pipefd[1]);
       // HERE WE SET TH META VARIABLES
        setenv("Ayoub","MAATOUCH ME",1);
        // setenv("CONTENT_LENGTH","",1);
        // setenv("CONTENT_TYPE","",1);
        setenv("PATH_INFO","/Users/aymaatou/Desktop/WebServer/server/public/index2.php",1);
        execv((char *const )location.cgi.c_str(), parm);
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
    // insert in a sting in cgi_content
    // s_cgi_content.insert(64, "Content-length: 34");
    // cgi_content = s_http  + map_status["200"] + "\r\n" + s_cgi_content;
    // std::cout << "--------------cgi_content----------" << std::endl;
    // std::cout << cgi_content << std::endl;
    // std::cout << "--------------cgi_content----------" << std::endl;
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
}
