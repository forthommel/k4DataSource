#include "k4DataSource/DataFormatter.h"
#include "k4DataSource/k4DataSourceItem.h"

k4DataSourceItem::k4DataSourceItem(const std::string& name, std::unique_ptr<DataFormatter> converter)
    : name_(name), converter_(std::move(converter)) {}

const std::vector<std::string>& k4DataSourceItem::inputs() const { return converter_->inputs(); }

const std::vector<std::string>& k4DataSourceItem::outputs() const { return converter_->outputs(); }

std::vector<void*> k4DataSourceItem::apply(const std::vector<void*>& input) {
  converter_->feed(input);
  return converter_->convert();
}
