#pragma once

#include "library.hpp"

int atoi(std::string num);
void list(std::string value, std::vector<std::string> *ptr, char delim);
int hex_to_dec(std::string num);
std::string lowerCase(std::string str);
std::string upperCase(std::string str);
std::string clean_whitespace(const std::string &s);
bool is_white_space(std::string str);
bool is_digit(std::string str);
int which_level(std::string str);
std::string intToString(int number);
std::string tmpname(void);
bool allowed_extension(std::string str);
int status_code(std::string status);