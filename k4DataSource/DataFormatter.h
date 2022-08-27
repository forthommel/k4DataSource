#ifndef k4DataSource_DataFormatter_h
#define k4DataSource_DataFormatter_h

#include <string>
#include <vector>

namespace ROOT {
  class RDataFrame;
}

class DataFormatter {
public:
  explicit DataFormatter(const std::vector<std::string>& columns_in = {},
                         const std::vector<std::string>& columns_out = {})
      : cols_in_(columns_in), cols_out_(columns_out) {}
  virtual ~DataFormatter() = default;

  virtual void convert(ROOT::RDataFrame&) = 0;

protected:
  const std::vector<std::string> cols_in_;
  const std::vector<std::string> cols_out_;
};

#endif
