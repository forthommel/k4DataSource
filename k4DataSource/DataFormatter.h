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
                         const std::vector<std::string>& columns_out = {});
  virtual ~DataFormatter() = default;

  const std::vector<std::string>& inputs() const { return cols_in_; }
  const std::vector<std::string>& outputs() const { return cols_out_; }

  void feed(const std::vector<void*>& input);

  virtual std::vector<void*> convert() = 0;

protected:
  const std::vector<std::string> cols_in_;
  const std::vector<std::string> cols_out_;

  std::vector<void*> input_data_;
};

#endif
