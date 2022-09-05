#include <iostream>
#include <locale>

#include "k4DataSource/k4Logger.h"

std::mutex gMutex;

k4Logger::k4Logger(const std::string& from, const std::string& file, size_t line_num)
    : from_(from), file_(file), line_num_(line_num) {}

k4Logger::k4Logger(const k4Logger& oth)
    : from_(oth.from_), file_(oth.file_), line_num_(oth.line_num_), stream_(oth.stream_.str()) {}

k4Logger::~k4Logger() {
  std::lock_guard<std::mutex> guard(gMutex);
  std::cout << "[" << now() << "] " << from_ << " (" << file_ << ":" << line_num_ << ") " << stream_.str() << std::endl;
}

std::string k4Logger::message() const { return stream_.str(); }

std::string k4Logger::now(const std::string& fmt) {
  auto now = std::time(nullptr);
  auto tm = *std::localtime(&now);
  char out_str[50];
  strftime(out_str, 50, fmt.c_str(), &tm);
  return std::string(out_str);
}

k4Exception::k4Exception(const std::string& from, const std::string& file, size_t line_num)
    : k4Logger(from, file, line_num), std::runtime_error(from) {}
