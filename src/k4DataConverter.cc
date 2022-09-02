#include <TClass.h>

#include <ROOT/RDF/Utils.hxx>
#include <iostream>
#include <stdexcept>

#include "k4DataSource/k4DataConverter.h"

k4DataConverter::k4DataConverter() {}

k4DataConverter::~k4DataConverter() {
  for (auto& out : outputs_)
    out.second.collection = nullptr;
}

void k4DataConverter::feed(const std::vector<void*>& input) {
  if (input.size() != cols_in_.size())
    throw std::runtime_error("Invalid inputs multiplicity:\n  expected: " + std::to_string(cols_in_.size()) +
                             ",\n  got: " + std::to_string(input.size()) + ".");
  for (size_t i = 0; i < cols_in_.size(); ++i)
    std::memcpy(inputs_.at(cols_in_.at(i)).collection, input.at(i), inputs_.at(cols_in_.at(i)).size);
}

std::unordered_map<std::string, void*> k4DataConverter::extract() const {
  std::unordered_map<std::string, void*> out;
  for (const auto& var : cols_out_)
    out[var] = (void*)&outputs_.at(var).collection;
  return out;
}

void k4DataConverter::describe() const {
  const auto sep = std::string(60, '=');
  std::cout << sep << "\nModule with input(s):";
  for (const auto& mod : inputs_)
    std::cout << "\n  * " << mod.first << " (" << mod.second.classType()->GetName() << ")";
  std::cout << "\n"
            << "and with output(s):";
  for (const auto& mod : outputs_)
    std::cout << "\n  * " << mod.first << " (" << mod.second.classType()->GetName() << ")";
  std::cout << "\n" << sep << std::endl;
}

void k4DataConverter::throwFailedToConsume(const std::type_info& tid, const std::string& label) const {
  throw std::runtime_error("Failed to consume " +
                           std::string(TClass::GetClass(ROOT::Internal::RDF::TypeID2TypeName(tid).c_str())->GetName()) +
                           " collection " + (label.empty() ? "" : "with label '" + label + "'") + " in event record.");
}

void k4DataConverter::throwFailedToPut(const std::type_info& tid, const std::string& label) const {
  throw std::runtime_error("Failed to put " +
                           std::string(TClass::GetClass(ROOT::Internal::RDF::TypeID2TypeName(tid).c_str())->GetName()) +
                           +" collection " + (label.empty() ? "" : "with label '" + label + "'") + " in event record.");
}

const TClass* k4DataConverter::Collection::classType() const {
  return TClass::GetClass(ROOT::Internal::RDF::TypeID2TypeName(type_info).c_str());
}
