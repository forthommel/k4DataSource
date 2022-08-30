#ifndef k4DataSource_k4DataSourceItem_h
#define k4DataSource_k4DataSourceItem_h

#include <memory>
#include <string>
#include <vector>

class DataFormatter;

class k4DataSourceItem {
public:
  explicit k4DataSourceItem(const std::string&, std::unique_ptr<DataFormatter>);

  const std::string& name() const { return name_; }
  const DataFormatter& converter() const { return *converter_; }
  std::vector<void*> apply(const std::vector<void*>&);

private:
  const std::string name_;
  std::unique_ptr<DataFormatter> converter_;
};

#endif
