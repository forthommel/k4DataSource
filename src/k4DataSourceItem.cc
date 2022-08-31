#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataSourceItem.h"

k4DataSourceItem::k4DataSourceItem(const std::string& name, std::unique_ptr<k4DataConverter> converter)
    : name_(name), converter_(std::move(converter)) {}

std::vector<k4Handle> k4DataSourceItem::apply(const std::vector<k4Handle>& input) {
  converter_->feed(input);
  converter_->convert();
  return converter_->extract();
}
