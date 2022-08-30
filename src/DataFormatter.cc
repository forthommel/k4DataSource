#include <stdexcept>

#include "k4DataSource/DataFormatter.h"

void DataFormatter::feed(const std::vector<void*>& input) {
  if (input.size() != cols_in_.size())
    throw std::runtime_error("Invalid inputs multiplicity: " + std::to_string(input.size()) +
                             " != " + std::to_string(cols_in_.size()) + "!");
  for (size_t i = 0; i < cols_in_.size(); ++i)
    memcpy(input_data_.at(cols_in_.at(i)), input.at(i), input_size_.at(cols_in_.at(i)));
}

std::vector<void*> DataFormatter::extract() const {
  std::vector<void*> out;
  for (const auto& var : cols_out_) {
    out.emplace_back(new char[output_size_.at(var)]);
    memcpy(out.back(), output_coll_.at(var), output_size_.at(var));
  }
  return out;
}
