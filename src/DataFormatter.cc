#include <stdexcept>

#include "k4DataSource/DataFormatter.h"

DataFormatter::DataFormatter(const std::vector<std::string>& columns_in, const std::vector<std::string>& columns_out)
    : cols_in_(columns_in), cols_out_(columns_out) {}

void DataFormatter::feed(const std::vector<void*>& input) {
  if (input.size() != cols_in_.size())
    throw std::runtime_error("Invalid inputs multiplicity: " + std::to_string(input.size()) +
                             " != " + std::to_string(cols_in_.size()) + "!");
  input_data_ = input;
}
