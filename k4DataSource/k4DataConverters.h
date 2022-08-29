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

class k4DataConverters {
public:
  static k4DataConverters& get();

  k4DataConverters(const k4DataConverters&) = delete;
  virtual ~k4DataConverters() = default;
  void operator=(k4DataConverters&) = delete;

  std::vector<std::string> converters() const;

  template <typename T>
  void registerConverter(const std::string& name) {
    static_assert(std::is_base_of<DataFormatter, T>::value,
                  "\n\n  *** Failed to register an object with improper inheritance into the factory. ***\n");
    converters_[name] = &build<T>;
  }

  std::unique_ptr<DataFormatter> build(const std::string& name) const;

private:
  k4DataConverters() {}

  template <typename T>
  static std::unique_ptr<DataFormatter> build() {
    return std::unique_ptr<DataFormatter>(new T);
  }

  typedef std::unique_ptr<DataFormatter> (*Converter)();

  std::unordered_map<std::string, Converter> converters_;
};

#endif
