#ifndef k4DataSource_k4DataConverterFactory_h
#define k4DataSource_k4DataConverterFactory_h

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4Parameters.h"

#define BUILDERNM(obj) obj##Builder
#define REGISTER_CONVERTER(name, obj)                                                \
  struct BUILDERNM(obj) {                                                            \
    BUILDERNM(obj)() { k4DataConverterFactory::get().registerConverter<obj>(name); } \
  };                                                                                 \
  static const BUILDERNM(obj) gk4DataConverter##obj;

/// A (singleton) lookup table with holding a "name" -> data formatter object constructor mapping
class k4DataConverterFactory {
public:
  /// Get a singleton of the data formatters lookup table
  static k4DataConverterFactory& get();

  k4DataConverterFactory(const k4DataConverterFactory&) = delete;
  virtual ~k4DataConverterFactory() = default;
  void operator=(k4DataConverterFactory&) = delete;

  /// List of data formatters registered in the lookup table
  std::vector<std::string> converters() const;
  /// Register a data formatter into the singleton lookup table
  template <typename T>
  inline void registerConverter(const std::string& name) {
    static_assert(std::is_base_of<k4DataConverter, T>::value,
                  "\n\n  *** Failed to register an object with improper inheritance into the factory. ***\n");
    converters_[name] = &build<T>;
  }
  /// Build a data formatter with a given set of parameters
  std::unique_ptr<k4DataConverter> build(const k4Parameters& = k4Parameters()) const;

private:
  k4DataConverterFactory() {}

  /// Recipe to build a data formatter
  template <typename T>
  static inline std::unique_ptr<k4DataConverter> build(const k4Parameters& params) {
    return std::unique_ptr<k4DataConverter>(new T(params));
  }

  /// Mockup of a data formatter constructor
  typedef std::unique_ptr<k4DataConverter> (*Converter)(const k4Parameters&);
  /// A collection of data formatters builders
  std::unordered_map<std::string, Converter> converters_;
};

#endif
