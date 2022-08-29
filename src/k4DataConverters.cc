#include "k4DataSource/k4DataConverters.h"

k4DataConverters& k4DataConverters::get() {
  static k4DataConverters conv;
  return conv;
}

std::vector<std::string> k4DataConverters::converters() const {
  std::vector<std::string> convs;
  for (const auto& conv : converters_)
    convs.emplace_back(conv.first);
  return convs;
}

std::unique_ptr<DataFormatter> k4DataConverters::build(const std::string& name) const { return converters_.at(name)(); }
