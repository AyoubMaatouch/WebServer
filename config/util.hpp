#pragma once

#include "library.hpp"

int ft_atoi(std::string num);
void list(std::string value, std::vector<std::string> *ptr, char delim);
int hex_to_dec(std::string num);
std::string lowerCase(std::string str);
std::string upperCase(std::string str);
std::string clean_whitespace(const std::string &s);
bool is_white_space(std::string str);
bool is_aligned(std::string str);
bool is_digit(std::string str);