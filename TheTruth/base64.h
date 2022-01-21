#pragma once


#include <vector>
#include <string>

using std::string;
using std::vector;

string base64encode(const void* buffer, size_t len);
vector<char> base64decode(string const& base64);