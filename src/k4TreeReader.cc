#include <ROOT/RDataFrame.hxx>
#include <iostream>

#include "k4DataSource/k4TreeReader.h"

k4TreeReader::k4TreeReader(const std::string& source, const std::vector<std::string>& filenames)
    : source_(source), filenames_(filenames) {
  TChain chain_model(source.data());
  for (const auto& filename : filenames_)
    chain_model.Add(filename.data());
  for (size_t i = 0; i < chain_model.GetListOfBranches()->GetEntries(); ++i) {
    std::string branch_name = chain_model.GetListOfBranches()->At(i)->GetName();
    const auto type_name =
        ROOT::Internal::RDF::ColumnName2ColumnTypeName(branch_name, &chain_model, nullptr, nullptr, false);
    TClass::GetClass(type_name.data());  // just in case supporting library is not yet loaded
    branch_names_.emplace_back(branch_name);
    branches_[branch_name] = BranchInfo{branch_name, type_name, {}};
    std::cout << ">>>> " << branch_name << " ::: " << type_name << std::endl;
  }
  std::cout << __PRETTY_FUNCTION__ << " >>> initialised!" << std::endl;
}

bool k4TreeReader::has(const std::string& name) const { return branches_.count(name) > 0; }

const std::string& k4TreeReader::typeName(const std::string& name) const {
  if (!has(name))
    throw std::runtime_error("Input tree does not have branch name '" + name + "'!");
  return branches_.at(name).type;
}

void k4TreeReader::setNumSlots(size_t num_slot) {
  for (auto& bi : branches_)
    bi.second.addresses = std::vector<void*>(num_slot, nullptr);
  chains_.resize(num_slot);
}

void k4TreeReader::init() {}

void k4TreeReader::initEntry(size_t slot, unsigned long long entry) {
  auto chain = std::make_unique<TChain>(source_.data());
  chain->ResetBit(kMustCleanup);
  for (const auto& filename : filenames_)
    chain->Add(filename.data());
  for (auto& bi : branches_) {
    auto& branch_info = bi.second;
    auto& addr = branch_info.addresses[slot];
    const auto type_class = TClass::GetClass(typeName(branch_info.name).data());
    if (type_class)
      chain->SetBranchAddress(branch_info.name.data(), &addr, nullptr, type_class, EDataType(0), true);
    else {
      if (!addr) {
        addr = new double();
        //fAddressesToFree.emplace_back((double*)addr);
      }
      chain->SetBranchAddress(branch_info.name.data(), addr);
    }
  }
  chain->AddBranchToCache("*", true);
  chain->GetEntry(entry);
  chains_[slot] = std::move(chain);
}

std::vector<void*> k4TreeReader::read(const std::string& name, const std::type_info& tid) {
  const auto& type = typeName(name);  // possibly throws
  const auto& req_tid = ROOT::Internal::RDF::TypeName2TypeID(type);
  if (req_tid != tid)
    throw std::runtime_error("Mismatching column type for '" + name + "': " + req_tid.name() + " != " + tid.name() +
                             "!");

  auto& bi = branches_.at(name);
  std::vector<void*> out(chains_.size());  // as many as slots
  for (size_t i = 0; i < chains_.size(); ++i)
    out[i] = (void*)&bi.addresses[i];
  return out;
}
