#pragma once

#include <vector>
#include <string>


std::vector<std::string> split_string(const std::string& str, const std::string& delimiter);
void replace_all(std::string& source, std::string const& find, std::string const& replace);

int min_int(int a, int b);