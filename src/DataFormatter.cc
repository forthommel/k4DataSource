#include <TClass.h>

#include <iostream>
#include <stdexcept>

#include "k4DataSource/DataFormatter.h"

void DataFormatter::feed(const std::vector<void*>& input) {
  if (input.size() != cols_in_.size())
    throw std::runtime_error("Invalid inputs multiplicity:\n  expected: " + std::to_string(cols_in_.size()) +
                             ",\n  got: " + std::to_string(input.size()) + ".");
  for (size_t i = 0; i < cols_in_.size(); ++i) {
    //inputs_[cols_in_.at(i)].collection = input.at(i);
    memcpy(inputs_.at(cols_in_.at(i)).collection, input.at(i), inputs_.at(cols_in_.at(i)).size);
    std::cout << i << ":::" << input.at(i) << "=>" << inputs_.at(cols_in_.at(i)).collection << std::endl;
  }
}

std::vector<void*> DataFormatter::extract() const {
  std::vector<void*> out;
  for (const auto& var : cols_out_) {
    out.emplace_back(new char[outputs_.at(var).size]);
    memcpy(out.back(), outputs_.at(var).collection, outputs_.at(var).size);
  }
  return out;
}

void DataFormatter::describe() const {
  const auto sep = std::string(60, '=');
  std::cout << sep << "\nModule with input(s):";
  for (const auto& mod : inputs_) {
    std::cout << "\n  * " << mod.first;
    if (mod.second.class_type)
      std::cout << " (" << mod.second.class_type->GetName() << ")";
  }
  std::cout << "\n"
            << "and with output(s):";
  for (const auto& mod : outputs_) {
    std::cout << "\n  * " << mod.first;
    if (mod.second.class_type)
      std::cout << " (" << mod.second.class_type->GetName() << ")";
  }
  std::cout << "\n" << sep << std::endl;
}
