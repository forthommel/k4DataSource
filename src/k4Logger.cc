#include <csignal>
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

k4Logger& k4Logger::enableDebugging(const std::string& rule) {
  enabled_dbg_.emplace_back(rule);
  return *this;
}

bool k4Logger::debuggingEnabled(const std::string& rule) const {
  if (enabled_dbg_.empty())
    return false;
  for (const auto& rule_test : enabled_dbg_)
    try {
      if (std::regex_match(rule, rule_test))
        return true;
    } catch (const std::regex_error& err) {
      throw std::runtime_error("Failed to evaluate regex for logging tool.\n\t" + std::string(err.what()));
    }
  return false;
}

std::string k4Logger::now(const std::string& fmt) {
  auto now = std::time(nullptr);
  auto tm = *std::localtime(&now);
  char out_str[50];
  std::strftime(out_str, 50, fmt.c_str(), &tm);
  return std::string(out_str);
}

//---------------------------------------------------------

k4Message::k4Message(const k4Message::Type& type,
                     const std::string& from,
                     const std::string& file,
                     size_t line_num) noexcept
    : type_(type), from_(from), file_(file), line_num_(line_num) {}

k4Message::k4Message(const k4Message& oth) noexcept
    : type_(oth.type_), from_(oth.from_), file_(oth.file_), line_num_(oth.line_num_), stream_(oth.stream_.str()) {}

k4Message::~k4Message() noexcept { dump(); }

void k4Message::dump(std::ostream* os) const noexcept {
  if (!os)
    os = k4Logger::get().stream();
  std::lock_guard<std::mutex> guard(k4Logger::get().mutex);
  (*os) << "[" << k4Logger::get().now() << "] ";
  if (type_ == mDebug)
    (*os) << "DEBUG " << from_ << " (" << file_ << ":" << line_num_ << ") ";
  (*os) << stream_.str() << std::endl;
}

std::string k4Message::message() const { return stream_.str(); }

//---------------------------------------------------------

k4Exception::k4Exception(const std::string& from, const std::string& file, size_t line_num) noexcept
    : k4Message(k4Message::mInfo, from, file, line_num), std::runtime_error("k4Exception") {}

k4Exception::k4Exception(const k4Exception& oth) noexcept : k4Message(oth), std::runtime_error("k4Exception") {}

k4Exception::~k4Exception() noexcept {
  dump();
  if (raise(SIGINT) != 0)
    exit(0);
}

void k4Exception::dump(std::ostream* os) const noexcept {
  if (!os)
    os = k4Logger::get().stream();
  std::lock_guard<std::mutex> guard(k4Logger::get().mutex);
  static const std::string separator(80, '-');
  (*os) << separator << "\n"
        << "[" << k4Logger::get().now() << "] "
        << "ERROR\n"
        << stream_.str() << "\n"
        << separator << "\n";
  if (from_.empty() && file_.empty())
    return;
  if (!from_.empty())
    (*os) << "Encountered from:\n"
          << "  " << from_ << "\n";
  if (!file_.empty()) {
    (*os) << "File: " << file_;
    if (line_num_ >= 0)
      (*os) << " at line " << line_num_;
    (*os) << "\n";
  }
  (*os) << separator << "\n";
}

const char* k4Exception::what() const noexcept {
  dump();
  return "k4Exception";
}
