#include "library.hpp"

void list(std::string value, std::vector<std::string> *ptr, char delim)
{
	std::stringstream ss_value(value);
	while (std::getline(ss_value, value, delim))
	{
		value = clean_whitespace(value);

		if (value.size())
		{
			ptr->push_back(value);
		}
	}
}

std::string upperCase(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		str[i] = toupper(str[i]);
	}
	return str;
}

std::string lowerCase(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		str[i] = tolower(str[i]);
	}
	return str;
}

int hex_to_dec(std::string num)
{
	int len = num.size();
	int base = 1;
	int temp = 0;

	num = upperCase(num);

	for (int i = len - 1; i >= 0; i--)
	{
		if (num[i] >= '0' && num[i] <= '9')
		{
			temp += (num[i] - 48) * base;
			base = base * 16;
		}
		else if (num[i] >= 'A' && num[i] <= 'F')
		{
			temp += (num[i] - 55) * base;
			base = base * 16;
		}
	}
	return temp;
}

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string clean_whitespace(const std::string &s)
{
	return rtrim(ltrim(s));
}

bool is_white_space(std::string str)
{
	if (str.find_last_not_of(WHITESPACE) == std::string::npos)
		return (true);
	return (false);
}

bool is_aligned(std::string str)
{
	if (str.size() < 3 || str[0] != '\t' || str[1] != '\t')
		return false;
	return true;
}

bool is_digit(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!isdigit(str[i]))
			return (false);
	}
	return (true);
}

int which_level(std::string str)
{
	int level = 0;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] != '\t')
			break;
		level++;
	}
	return (level);
}

std::string intToString(int number)
{
	std::stringstream ss;
	std::string output;

	ss << number;
	ss >> output;
	return output;
}

std::string tmpname(void)
{
	static int i = 0;
	i++;
	return "tmp_files/body-" + intToString(i);
}