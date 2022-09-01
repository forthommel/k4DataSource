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
  chain_->SetBranchStatus("*", false);
  for (size_t i = 0; i < chain_->GetListOfBranches()->GetEntries(); ++i) {
    const std::string branch_name = chain_->GetListOfBranches()->At(i)->GetName();
    const auto type_name =
        ROOT::Internal::RDF::ColumnName2ColumnTypeName(branch_name, chain_.get(), nullptr, nullptr, false);
    TClass::GetClass(type_name.c_str());  // just in case supporting library is not yet aoaded
    branches_.insert(std::make_pair(branch_name, BranchInfo{branch_name, type_name, {}}));
    auto& branch_info = branches_.at(branch_name);
    auto& addr = branch_info.addresses.emplace_back();
    if (const auto type_class = TClass::GetClass(branch_info.type.c_str()))
      chain_->SetBranchAddress(
          // linking pre-booked memory to tree contents
          branch_info.name.c_str(),
          &addr,
          type_class,
          EDataType::kNoType_t,
          true);
    else {
      if (!addr) {
        dangling_ptrs_.emplace_back(std::make_unique<double>());
        addr = dangling_ptrs_.rbegin()->get();
      }
      chain_->SetBranchAddress(branch_info.name.c_str(), addr.get());
    }
    chain_->SetBranchStatus(branch_info.name.c_str(), true);
  }
  for (const auto& converter : converters) {
    auto conv = k4DataConverterFactory::get().build(converter);
    std::cout << ">>> added " << converter << std::endl;
    for (const auto& in_coll : conv->inputs())
      conv->setInputType(in_coll, TClass::GetClass(branches_.at(in_coll).type.c_str()));
    for (const auto& out_coll : conv->outputs()) {
      std::cout << ">>>>>>>>>> producing collection " << out_coll << std::endl;
      branches_.insert(std::make_pair(out_coll, BranchInfo{out_coll, converter, {}}));
      //conv.setOutputType(out_coll, TClass::GetClass(GetTypeName(out_coll).c_str()));
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
  //if (entry == 0 || entry != current_entry_.at(slot)) {
  auto ret = chain_->GetEntry(event);
  current_entry_ = event;
  return ret > 0;
}

const k4Record& k4SlotReader::read(const std::string& name, const std::type_info& tid) const {
  const auto& req_tid = ROOT::Internal::RDF::TypeName2TypeID(branches_.at(name).type);  // NO copy
  if (req_tid != tid)
    throw std::runtime_error("Invalid type requested for column '" + name + "':\n  expected " + req_tid.name() +
                             ",\n  got " + tid.name() + ".");
  return branches_.at(name).addresses;
}
