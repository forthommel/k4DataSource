#include <TChain.h>

#include <ROOT/RDF/Utils.hxx>

#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"
#include "k4DataSource/k4Parameters.h"
#include "k4DataSource/k4SlotReader.h"

k4SlotReader::k4SlotReader(const std::string& source,
                           const std::vector<std::string>& filenames,
                           const std::vector<k4Parameters>& converters)
    : chain_(new TChain(source.c_str())) {
  for (const auto& filename : filenames)
    chain_->Add(filename.c_str());
  for (size_t i = 0; i < chain_->GetListOfBranches()->GetEntries(); ++i) {
    const std::string branch_name = chain_->GetListOfBranches()->At(i)->GetName();
    const auto type_name =
        ROOT::Internal::RDF::ColumnName2ColumnTypeName(branch_name, chain_.get(), nullptr, nullptr, false);
    TClass::GetClass(type_name.c_str());  // just in case supporting library is not yet added
    branches_.insert(std::make_pair(branch_name, BranchInfo{true, branch_name, type_name, nullptr}));
    auto& branch_info = branches_.at(branch_name);
    auto* type_class = TClass::GetClass(branch_info.type.c_str());
    if (!type_class)
      throw std::runtime_error("Output format '" + branch_info.type + "' is not defined for branch '" +
                               branch_info.name + "'. Please generate the readout dictionary accordingly.");
    auto& addr = branch_info.address;
    chain_->SetBranchAddress(
        // linking pre-booked memory to tree contents
        branch_info.name.c_str(),
        &addr,
        type_class,
        EDataType::kNoType_t,
        true);
    chain_->SetBranchStatus(branch_info.name.c_str(), true);
  }
  for (const auto& converter : converters) {
    auto conv = k4DataConverterFactory::get().build(converter);
    for (const auto& out_coll : conv->outputs()) {
      branches_.insert(std::make_pair(out_coll,
                                      BranchInfo{false,
                                                 out_coll,
                                                 ROOT::Internal::RDF::TypeID2TypeName(conv->outputType(out_coll)),
                                                 conv->output(out_coll)}));
    }
    conv->describe();
    converters_[converter.name<std::string>()] = std::move(conv);
  }
}

std::vector<std::string> k4SlotReader::branches() const {
  std::vector<std::string> out;
  for (const auto& branch : branches_)
    out.emplace_back(branch.first);
  for (const auto& conv : converters_)
    for (const auto& out_coll : conv.second->outputs())
      out.emplace_back(out_coll);
  return out;
}

const k4SlotReader::BranchInfo& k4SlotReader::branchInfo(const std::string& branch) const {
  if (branches_.count(branch) == 0)
    throw std::runtime_error("Failed to retrieve info about branch with name '" + branch + "'.");
  return branches_.at(branch);
}

bool k4SlotReader::initEntry(unsigned long long event) {
  if (event == current_event_)
    return true;
  auto ret = chain_->GetEntry(event);
  current_event_ = event;
  for (auto& col : converters_) {
    auto& conv = col.second;
    // prepare all input collections for the converter
    std::vector<void*> inputs;
    for (const auto& input : conv->inputs()) {
      const auto& info = branchInfo(input);
      inputs.emplace_back(info.in_tree ? info.address                         // first browse the input tree branches
                                       : read(input, conv->inputType(input))  // then check the converters outputs
      );
    }
    // launch the conversion
    conv->feed(inputs);
    conv->convert();
  }
  return ret > 0;
}

void* k4SlotReader::read(const std::string& name, const std::type_info& tid) const {
  const auto& req_tid = ROOT::Internal::RDF::TypeName2TypeID(branches_.at(name).type);  // NO copy
  if (req_tid != tid)
    throw std::runtime_error("Invalid type requested for column '" + name + "':\n  expected " + req_tid.name() +
                             ",\n  got " + tid.name() + ".");
  // first loop over the various converters and spot if one produces the column name
  for (auto& col : converters_) {
    auto& conv = col.second;
    const auto& mod_outputs = conv->outputs();
    if (std::find(mod_outputs.begin(), mod_outputs.end(), name) == mod_outputs.end())
      continue;
    // found corresponding module ; will start conversion of inputs
    return conv->extract().at(name);
  }
  // then check if the input tree has the corresponding branch
  if (branches_.count(name) > 0) {
    if (!branches_.at(name).address)
      throw std::runtime_error("Failed to retrieve branch '" + name + "' from input tree.");
    return (void*)&branches_.at(name).address;
  }
  // otherwise, throw
  throw std::runtime_error("Failed to retrieve column '" + name +
                           "', neither in conversion modules outputs nor in input tree columns.");
}
