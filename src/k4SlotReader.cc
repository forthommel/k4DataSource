#include <TChain.h>

#include <ROOT/RDF/Utils.hxx>
#include <iostream>

#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"
#include "k4DataSource/k4SlotReader.h"

k4SlotReader::k4SlotReader(const std::string& source,
                           const std::vector<std::string>& filenames,
                           const std::vector<std::string>& converters,
                           const EventRange& range)
    : range_(range), chain_(new TChain(source.c_str())) {
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
    converters_[converter] = std::move(conv);
  }
  chain_->GetEntry(range_.first);  // start by loading the first entry into memory
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
  if (event < range_.first || event >= range_.second) {
    std::cout << "Warning: requesting event outside [" << range_.first << ", " << range_.second << "[ range";
    return false;
  }
  std::cout << __PRETTY_FUNCTION__ << "::: " << event << std::endl;
  if (event > 0 && event == current_event_)
    return true;
  auto ret = chain_->GetEntry(event);
  current_event_ = event;
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
    conv->describe();
    // found corresponding module ; will start conversion of inputs
    const auto& mod_inputs = conv->inputs();
    std::vector<void*> inputs(mod_inputs.size(), nullptr);
    for (size_t i = 0; i < mod_inputs.size(); ++i) {
      const auto& info = branchInfo(mod_inputs.at(i));
      if (info.in_tree)
        inputs[i] = info.address;  // first browse the input tree branches
      else
        inputs[i] = read(mod_inputs.at(i), conv->inputType(mod_inputs.at(i)));  // then check the converters
    }
    conv->feed(inputs);
    conv->convert();
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
