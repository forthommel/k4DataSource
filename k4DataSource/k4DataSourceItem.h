#ifndef k4DataSource_k4DataSourceItem_h
#define k4DataSource_k4DataSourceItem_h

#include <memory>
#include <string>
#include <vector>

class k4DataConverter;
class k4Handle;

class k4DataSourceItem {
public:
  explicit k4DataSourceItem(const std::string&, std::unique_ptr<k4DataConverter>);
  /// Name of the converted objects collection
  const std::string& name() const { return name_; }

  /// Converter object reference
  k4DataConverter& converter() { return *converter_; }
  /// Constant-qualified qualifier object reference
  const k4DataConverter& converter() const { return *converter_; }
  /// Run the converter on a list of variables
  std::vector<k4Handle> apply(const std::vector<k4Handle>&) const;

private:
  const std::string name_;
  std::unique_ptr<k4DataConverter> converter_;
};

#endif
