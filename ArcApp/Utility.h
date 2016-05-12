#pragma once
#include "dirent.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>


std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
