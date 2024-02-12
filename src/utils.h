#ifndef TCP_PROXY_SERVER_UTILS_H_
#define TCP_PROXY_SERVER_UTILS_H_

#include <cstring>
#include <fstream>
#include <iostream>

namespace utils {
void OutMessage(const std::string& message);
void WriteLog(const std::string& filename, const std::string& log_text);
void CheckResult(int result, const std::string& log_text);
void ExitWithLog(const std::string& log_text);
}  // namespace utils

#endif  // TCP_PROXY_SERVER_UTILS_H_
