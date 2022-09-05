#ifndef k4DataSource_k4Logger_h
#define k4DataSource_k4Looger_h

#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

extern std::mutex gMutex;

class k4Logger {
public:
  explicit k4Logger(const std::string&, const std::string&, size_t);
  k4Logger(const k4Logger&);
  virtual ~k4Logger();

  std::string message() const;

  template <typename T>
  inline friend const k4Logger& operator<<(const k4Logger& log, const T& var) noexcept {
    auto& nc_log = const_cast<k4Logger&>(log);
    nc_log.stream_ << var;
    return log;
  }

  template <typename T, typename U>
  inline friend const k4Logger& operator<<(const k4Logger& log, const std::pair<T, U>& pair_var) noexcept {
    return log << "(" << pair_var.first << ", " << pair_var.second << ")";
  }

  template <typename T>
  inline friend const k4Logger& operator<<(const k4Logger& log, const std::vector<T>& vec_var) noexcept {
    log << "{";
    std::string sep;
    for (const auto& var : vec_var)
      log << sep << var, sep = ", ";
    return log << "}";
  }

  template <typename T, typename U>
  inline friend const k4Logger& operator<<(const k4Logger& log, const std::map<T, U>& map_var) noexcept {
    log << "{";
    std::string sep;
    for (const auto& var : map_var)
      log << sep << "{" << var.first << " -> " << var.second << "}", sep = ", ";
    return log << "}";
  }

  inline friend const k4Logger& operator<<(const k4Logger& log, std::ios_base& (*f)(std::ios_base&)) noexcept {
    auto& nc_log = const_cast<k4Logger&>(log);
    f(nc_log.stream_);
    return log;
  }

  template <typename T>
  inline const k4Logger& log(T&& lam) noexcept {
    lam(*this);
    return *this;
  }

private:
  static std::string now(const std::string& = "%H:%M:%S");

  const std::string from_, file_;
  const size_t line_num_;
  std::ostringstream stream_;
};

struct k4Exception : public k4Logger, public std::runtime_error {
  explicit k4Exception(const std::string&, const std::string&, size_t);
};

struct k4EmptyLogger {
  k4EmptyLogger() = default;
  ~k4EmptyLogger() = default;

  template <class T>
  k4EmptyLogger& operator<<(const T&) {
    return *this;
  }
  template <typename T>
  k4EmptyLogger& log(T&&) {
    return *this;
  }
};

#ifdef _WIN32
#define __FUNC__ __FUNCSIG__
#else
#define __FUNC__ __PRETTY_FUNCTION__
#endif

#define k4Log k4Logger(__FUNC__, __FILE__, __LINE__)
#define k4Error k4Log
#ifdef DEBUG
#define k4Debug k4Log
#else
#define k4Debug k4EmptyLogger
#endif

#endif
