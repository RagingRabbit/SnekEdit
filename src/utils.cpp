#include "utils.h"

#include "editor.h"


std::vector<text_line> split_string(const std::string& str, const std::string& delimiter)
{
	std::vector<text_line> strings;

	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = str.find(delimiter, prev)) != std::string::npos)
	{
		strings.push_back({ str.substr(prev, pos - prev) });
		prev = pos + 1;
	}

	strings.push_back({ str.substr(prev) });

	return strings;
}

void replace_all(std::string& source, std::string const& find, std::string const& replace)
{
	for (std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
	{
		source.replace(i, find.length(), replace);
		i += replace.length();
	}
}

int min_int(int a, int b)
{
	return a < b ? a : b;
}