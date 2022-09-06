#include <iostream>
#include <locale>

#include "k4DataSource/k4Logger.h"

//---------------------------------------------------------

std::mutex k4Logger::mutex;

k4Logger& k4Logger::get(std::ostream* os) {
  static k4Logger logger(os);
  return logger;
}

k4Logger::k4Logger(std::ostream* os) : stream_(os) {
  if (!stream_)
    stream_ = &std::cout;
}

std::ostream* k4Logger::stream() {
  if (!stream_)
    throw std::runtime_error("Failed to initialise output stream for logger instance.");
  return stream_;
}

std::string k4Logger::now(const std::string& fmt) {
  auto now = std::time(nullptr);
  auto tm = *std::localtime(&now);
  char out_str[50];
  std::strftime(out_str, 50, fmt.c_str(), &tm);
  return std::string(out_str);
}

//---------------------------------------------------------

k4Message::k4Message(const std::string& from, const std::string& file, size_t line_num) noexcept
    : from_(from), file_(file), line_num_(line_num) {}

k4Message::k4Message(const k4Message& oth) noexcept
    : from_(oth.from_), file_(oth.file_), line_num_(oth.line_num_), stream_(oth.stream_.str()) {}

k4Message::~k4Message() noexcept { dump(); }

void k4Message::dump(std::ostream* os) const noexcept {
  if (!os)
    os = k4Logger::get().stream();
  std::lock_guard<std::mutex> guard(k4Logger::get().mutex);
  (*os) << "[" << k4Logger::get().now() << "] " << from_ << " (" << file_ << ":" << line_num_ << ") " << stream_.str()
        << std::endl;
}

std::string k4Message::message() const { return stream_.str(); }

//---------------------------------------------------------

k4Exception::k4Exception(const std::string& from, const std::string& file, size_t line_num) noexcept
    : k4Message(from, file, line_num), std::runtime_error("k4Exception") {}

k4Exception::~k4Exception() noexcept {}

const char* k4Exception::what() const noexcept {
  dump();
  return "k4Exception";
}
