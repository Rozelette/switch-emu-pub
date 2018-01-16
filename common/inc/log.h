#pragma once
#include <string>

namespace logger {

void error(std::string s, ...);
void warn(std::string s, ...);
void info(std::string s, ...);

} // namespace logger
