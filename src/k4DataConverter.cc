#include <TClass.h>

#include <iostream>
#include <stdexcept>

#include "k4DataSource/k4DataConverter.h"

k4DataConverter::k4DataConverter() {}

void k4DataConverter::feed(const std::vector<k4Handle>& input) {
  if (input.size() != cols_in_.size())
    throw std::runtime_error("Invalid inputs multiplicity:\n  expected: " + std::to_string(cols_in_.size()) +
                             ",\n  got: " + std::to_string(input.size()) + ".");
  for (size_t i = 0; i < cols_in_.size(); ++i) {
    //inputs_[cols_in_.at(i)].collection = input.at(i);
    memcpy(inputs_.at(cols_in_.at(i)).collection.get(), input.at(i).get(), inputs_.at(cols_in_.at(i)).size);
    std::cout << i << ":::" << input.at(i) << "=>" << inputs_.at(cols_in_.at(i)).collection << std::endl;
  }
}

std::vector<k4Handle> k4DataConverter::extract() const {
  std::vector<k4Handle> out;
  for (const auto& var : cols_out_) {
    out.emplace_back(new char[outputs_.at(var).size]);
    memcpy(out.back().get(), outputs_.at(var).collection.get(), outputs_.at(var).size);
  }
  return out;
}

void k4DataConverter::describe() const {
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
