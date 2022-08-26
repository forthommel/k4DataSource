#ifndef k4DataSource_k4DataConverters_h
#define k4DataSource_k4DataConverters_h

//#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#define BUILDERNM(obj) obj##Builder
#define REGISTER_CONVERTER(name, obj)                                                \
  namespace converter {                                                              \
    struct BUILDERNM(obj) {                                                          \
      /*BUILDERNM(obj)() { k4DataConverters::get().registerConverter<obj>(name); }*/ \
      BUILDERNM(obj)() { k4DataConverters::get().registerConverter(name, obj); }     \
    };                                                                               \
    static const BUILDERNM(obj) gk4DataConverter##obj;                               \
  }

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

  typedef ROOT::RDataFrame* (*Converter)(const ROOT::RDataFrame*);
  //typedef std::function<ROOT::RDataFrame*(const ROOT::RDataFrame*)> Converter;

  //template <typename T>
  void registerConverter(const std::string& name, Converter conv) { converters_[name] = conv; }

private:
  k4DataConverters() {}
  std::unordered_map<std::string, Converter> converters_;
};

#endif
