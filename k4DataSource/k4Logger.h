#ifndef k4DataSource_k4Logger_h
#define k4DataSource_k4Looger_h

#include <map>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// A (singleton) logger object
class k4Logger {
public:
  /// Get a singleton of the messages logger
  static k4Logger& get(std::ostream* = nullptr);

  k4Logger(const k4Logger&) = delete;
  virtual ~k4Logger() = default;
  void operator=(k4Logger&) = delete;

  std::ostream* stream();

  static std::mutex mutex;
  static std::string now(const std::string& = "%H:%M:%S");

private:
  k4Logger(std::ostream* = nullptr);
  std::ostream* stream_{nullptr};
};

class k4Message {
public:
  explicit k4Message(const std::string&, const std::string&, size_t) noexcept;
  k4Message(const k4Message&) noexcept;
  virtual ~k4Message() noexcept;

  void dump(std::ostream* = nullptr) const noexcept;
  std::string message() const;

  template <typename T>
  inline friend const k4Message& operator<<(const k4Message& log, const T& var) noexcept {
    auto& nc_log = const_cast<k4Message&>(log);
    nc_log.stream_ << var;
    return log;
  }

  template <typename T, typename U>
  inline friend const k4Message& operator<<(const k4Message& log, const std::pair<T, U>& pair_var) noexcept {
    return log << "(" << pair_var.first << ", " << pair_var.second << ")";
  }

  template <typename T>
  inline friend const k4Message& operator<<(const k4Message& log, const std::vector<T>& vec_var) noexcept {
    log << "{";
    std::string sep;
    for (const auto& var : vec_var)
      log << sep << var, sep = ", ";
    return log << "}";
  }

  template <typename T, typename U>
  inline friend const k4Message& operator<<(const k4Message& log, const std::map<T, U>& map_var) noexcept {
    log << "{";
    std::string sep;
    for (const auto& var : map_var)
      log << sep << "{" << var.first << " -> " << var.second << "}", sep = ", ";
    return log << "}";
  }

  inline friend const k4Message& operator<<(const k4Message& log, std::ios_base& (*f)(std::ios_base&)) noexcept {
    auto& nc_log = const_cast<k4Message&>(log);
    f(nc_log.stream_);
    return log;
  }

  template <typename T>
  inline const k4Message& log(T&& lam) noexcept {
    lam(*this);
    return *this;
  }

protected:
  const std::string from_, file_;
  const size_t line_num_;
  std::ostringstream stream_;
};

struct k4Exception : public k4Message, public std::runtime_error {
  explicit k4Exception(const std::string&, const std::string&, size_t) noexcept;
  virtual ~k4Exception() noexcept override;

  const char* what() const noexcept override;
};

struct k4EmptyLogger {
  k4EmptyLogger() noexcept = default;
  ~k4EmptyLogger() noexcept = default;

  template <class T>
  k4EmptyLogger& operator<<(const T&) noexcept {
    return *this;
  }
  template <typename T>
  k4EmptyLogger& log(T&&) noexcept {
    return *this;
  }
};

#ifdef _WIN32
#define __FUNC__ __FUNCSIG__
#else
#define __FUNC__ __PRETTY_FUNCTION__
#endif

#define k4Log k4Message(__FUNC__, __FILE__, __LINE__)
#define k4Error k4Exception(__FUNC__, __FILE__, __LINE__)
#ifdef DEBUG
#define k4Debug k4Log
#else
#define k4Debug k4EmptyLogger
#endif

#endif
