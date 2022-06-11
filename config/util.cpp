#include "library.hpp"

void list(std::string value, std::vector<std::string> *ptr, char delim)
{
	std::stringstream ss_value(value);
	while (std::getline(ss_value, value, delim))
	{
		value.erase(std::remove(value.begin(), value.end(), ' '), value.end());

		if (value.size())
		{
			ptr->push_back(value);
		}
	}
}

int ft_atoi(std::string num)
{
	int i = 0;

	for (std::string::iterator it = num.begin(); it != num.end(); it++)
	{
		i += (*it - 48);
		i *= 10;
	}
	return (i / 10);
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