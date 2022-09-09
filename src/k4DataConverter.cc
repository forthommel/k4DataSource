#include <TClass.h>

#include <ROOT/RDF/Utils.hxx>

#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4Logger.h"

k4DataConverter::k4DataConverter(const k4Parameters& params)
    : conv_name_(params.get<std::string>("output")), params_(params) {}

void k4DataConverter::feed(const std::vector<podio::CollectionReadBuffers>& input) {
  if (input.size() != cols_in_.size())
    throw k4Error << "Invalid inputs multiplicity:\n"
                  << "  expected: " << cols_in_.size() << " (" << cols_in_ << "),\n"
                  << "  got: " << input.size() << ".";
  for (size_t i = 0; i < cols_in_.size(); ++i)
    std::memcpy(inputs_.at(cols_in_.at(i)).collection.data, input.at(i).data, inputs_.at(cols_in_.at(i)).size);
}

std::unordered_map<std::string, podio::CollectionWriteBuffers> k4DataConverter::extract() const {
  std::unordered_map<std::string, podio::CollectionWriteBuffers> out;
  for (const auto& coll : outputs_)
    out[coll.first] = coll.second.collection;
  return out;
}

void k4DataConverter::describe() const {
  const auto sep = std::string(60, '=');
  k4Log.log([&](auto&& log) {
    log << "\n"
        << sep << "\n"
        << "Module with input(s):";
    for (const auto& mod : inputs_)
      log << "\n  * " << mod.first << " (" << mod.second.classType()->GetName() << ")";
    log << "\n"
        << "and with output(s):";
    for (const auto& mod : outputs_)
      log << "\n  * " << mod.first << " (" << mod.second.classType()->GetName() << ")";
    log << "\n" << sep;
  });
}

void k4DataConverter::throwFailedToConsume(const std::type_info& tid, const std::string& label) const {
  throw k4Error << "Failed to consume "
                << TClass::GetClass(ROOT::Internal::RDF::TypeID2TypeName(tid).c_str())->GetName() << " collection "
                << (label.empty() ? "" : "with label '" + label + "' ") << "in event record.";
}

void k4DataConverter::throwFailedToPut(const std::type_info& tid, const std::string& label) const {
  throw k4Error << "Failed to put " << TClass::GetClass(ROOT::Internal::RDF::TypeID2TypeName(tid).c_str())->GetName()
                << " collection " << (label.empty() ? "" : "with label '" + label + "' ") << "in event record.";
}

const TClass* k4DataConverter::Collection::classType() const {
  return TClass::GetClass(ROOT::Internal::RDF::TypeID2TypeName(type_info).c_str());
}
