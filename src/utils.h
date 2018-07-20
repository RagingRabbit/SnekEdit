#pragma once

#include <vector>
#include <string>


struct text_line;

std::vector<text_line> split_string(const std::string& str, const std::string& delimiter);
void replace_all(std::string& source, std::string const& find, std::string const& replace);

int min_int(int a, int b);