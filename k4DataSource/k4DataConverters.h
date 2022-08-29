#ifndef k4DataSource_k4DataConverters_h
#define k4DataSource_k4DataConverters_h

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "k4DataSource/DataFormatter.h"

#define BUILDERNM(obj) obj##Builder
#define REGISTER_CONVERTER(name, obj)                                          \
  struct BUILDERNM(obj) {                                                      \
    BUILDERNM(obj)() { k4DataConverters::get().registerConverter<obj>(name); } \
  };                                                                           \
  static const BUILDERNM(obj) gk4DataConverter##obj;

/// A (singleton) lookup table with holding a "name" -> data formatter object constructor mapping
class k4DataConverters {
public:
  /// Get a singleton of the data formatters lookup table
  static k4DataConverters& get();

  k4DataConverters(const k4DataConverters&) = delete;
  virtual ~k4DataConverters() = default;
  void operator=(k4DataConverters&) = delete;

  /// List of data formatters registered in the lookup table
  std::vector<std::string> converters() const;
  /// Register a data formatter into the singleton lookup table
  template <typename T>
  inline void registerConverter(const std::string& name) {
    static_assert(std::is_base_of<DataFormatter, T>::value,
                  "\n\n  *** Failed to register an object with improper inheritance into the factory. ***\n");
    converters_[name] = &build<T>;
  }
  /// Build a data formatter with a given name
  std::unique_ptr<DataFormatter> build(const std::string& name) const;

private:
  k4DataConverters() {}

  /// Recipe to build a data formatter
  template <typename T>
  static inline std::unique_ptr<DataFormatter> build() {
    return std::unique_ptr<DataFormatter>(new T);
  }

  /// Mockup of a data formatter constructor
  typedef std::unique_ptr<DataFormatter> (*Converter)();
  /// A collection of data formatters builders
  std::unordered_map<std::string, Converter> converters_;
};

#endif
