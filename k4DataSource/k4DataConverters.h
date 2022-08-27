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

namespace ROOT {
  class RDataFrame;
}

class k4DataConverters {
public:
  static k4DataConverters& get() {
    static k4DataConverters conv;
    return conv;
  }

  k4DataConverters(const k4DataConverters&) = delete;
  virtual ~k4DataConverters() = default;
  void operator=(k4DataConverters&) = delete;

  std::vector<std::string> converters() const {
    std::vector<std::string> convs;
    for (const auto& conv : converters_)
      convs.emplace_back(conv.first);
    return convs;
  }

  //typedef void (*Converter)(ROOT::RDataFrame&);
  //typedef std::function<ROOT::RDataFrame*(const ROOT::RDataFrame*)> Converter;
  typedef std::unique_ptr<DataFormatter> (*Converter)();

  template <typename T>
  void registerConverter(const std::string& name) {
    static_assert(std::is_base_of<DataFormatter, T>::value,
                  "\n\n  *** Failed to register an object with improper inheritance into the factory. ***\n");
    converters_[name] = &build<T>;
  }

  std::unique_ptr<DataFormatter> build(const std::string& name) const { return converters_.at(name)(); }

private:
  k4DataConverters() {}
  template <typename T>
  static std::unique_ptr<DataFormatter> build() {
    return std::unique_ptr<DataFormatter>(new T);
  }
  std::unordered_map<std::string, Converter> converters_;
};

#endif
