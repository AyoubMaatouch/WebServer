#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <map>
#include <string.h>
#include <sstream>
#include <fstream>
#include <sys/stat.h>


std::string	        get_content_type(std::string s);
std::string         to_string(int i);
bool                replace_in_uri(std::string& str, const std::string& from, const std::string& to);  
bool                isdir (std::string  &path);
std::string         get_index(std::string& ,std::vector<std::string>&);