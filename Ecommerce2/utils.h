#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <sstream> // For istringstream

// Helper to split string by delimiter
std::vector<std::string> split(const std::string& s, char delimiter);

#endif // UTILS_H