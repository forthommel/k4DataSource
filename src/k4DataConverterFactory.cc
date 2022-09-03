#include <stdexcept>

#include "k4DataSource/k4DataConverterFactory.h"
#include "k4DataSource/k4Parameters.h"

k4DataConverterFactory& k4DataConverterFactory::get() {
  static k4DataConverterFactory conv;
  return conv;
}

std::vector<std::string> k4DataConverterFactory::converters() const {
  std::vector<std::string> convs;
  for (const auto& conv : converters_)
    convs.emplace_back(conv.first);
  return convs;
}

std::unique_ptr<k4DataConverter> k4DataConverterFactory::build(const k4Parameters& params) const {
  const auto& name = params.name<std::string>();
  if (name.empty())
    throw std::runtime_error("Failed to retrieve a name field in data converter constructor parameters.");
  if (converters_.count(name) == 0)
    throw std::runtime_error("Failed to build a data converter with name '" + name + "'.");
  return converters_.at(name)(params);
}
