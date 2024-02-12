#include "utils.h"

void utils::OutMessage(const std::string& message) {
  std::cout << message << "\n";
}

void utils::WriteLog(const std::string& filename, const std::string& log_text) {
  std::ofstream out(filename.c_str(), std::ios::app);

  if (out.is_open()) {
    out << std::endl << log_text << std::endl;
    out.close();
  } else {
    utils::ExitWithLog("Error occured when open log file");
  }
}

void utils::CheckResult(int result, const std::string& log_text) {
  if (result < 0) {
    utils::ExitWithLog(log_text);
  }
}

void utils::ExitWithLog(const std::string& log_text) {
  if (errno) {
    std::cerr << log_text << ": " << std::strerror(errno) << std::endl;
    exit(errno);
  } else {
    std::cerr << log_text << std::endl;
    exit(EXIT_FAILURE);
  }
}
