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
  }
  num_entries_ = chain_model.GetEntries();
}

bool k4TreeReader::has(const std::string& name) const { return branches_.count(name) > 0; }

const std::string& k4TreeReader::typeName(const std::string& name) const {
  if (!has(name))
    throw std::runtime_error("Input tree does not have branch name '" + name + "'!");
  return branches_.at(name).type;
}

void k4TreeReader::setNumSlots(size_t num_slots) {
  num_slots = std::max(num_slots, size_t{1});
  for (auto& bi : branches_)
    bi.second.addresses = std::vector<void*>(num_slots, nullptr);
  for (size_t i = 0; i < num_slots; ++i) {
    auto chain = std::make_unique<TChain>(source_.data());
    chain->ResetBit(kMustCleanup);
    for (const auto& filename : filenames_)
      chain->Add(filename.data());
    for (auto& bi : branches_) {
      auto& branch_info = bi.second;
      auto& addr = branch_info.addresses[i];
      const auto type_class = TClass::GetClass(typeName(branch_info.name).data());
      if (type_class)
        chain->SetBranchAddress(branch_info.name.data(), &addr, nullptr, type_class, EDataType(0), true);
      else {
        if (!addr) {
          dangling_ptrs_.emplace_back(std::make_unique<double>());
          addr = dangling_ptrs_.rbegin()->get();
        }
        chain->SetBranchAddress(branch_info.name.data(), addr);
      }
    }
    chain->AddBranchToCache("*", true);
    chains_.emplace_back(std::move(chain));
    ranges_.resize(num_slots);
  }
}

void k4TreeReader::init() {
  const auto num_slots = ranges_.size();
  const auto expected_size = num_entries_ / num_slots,
             remaining = num_entries_ % num_slots;  // last remaining events after splitting
  for (size_t i = 0; i < num_slots; ++i)
    ranges_.emplace_back(std::make_pair(
        i * expected_size,                            // beginning of range
        (i + 1) * expected_size                       // end of range
            + (i < num_slots - 1 ? 0ull : remaining)  // last bit gets the remaining events
        ));
}

bool k4TreeReader::initEntry(size_t slot, unsigned long long entry) { return chains_.at(slot)->GetEntry(entry) > 0; }

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
