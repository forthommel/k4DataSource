#include <ROOT/RDataFrame.hxx>
#include <iostream>

#include "k4DataSource/k4TreeReader.h"

k4TreeReader::k4TreeReader(const std::string& source, const std::vector<std::string>& filenames)
    : source_(source), filenames_(filenames) {
  TChain chain_model(source.c_str());
  for (const auto& filename : filenames_)
    chain_model.Add(filename.c_str());
  for (size_t i = 0; i < chain_model.GetListOfBranches()->GetEntries(); ++i) {
    std::string branch_name = chain_model.GetListOfBranches()->At(i)->GetName();
    const auto type_name =
        ROOT::Internal::RDF::ColumnName2ColumnTypeName(branch_name, &chain_model, nullptr, nullptr, false);
    TClass::GetClass(type_name.c_str());  // just in case supporting library is not yet loaded
    branch_names_.emplace_back(branch_name);
    branches_[branch_name] = BranchInfo{branch_name, type_name, {nullptr}};
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
  num_slots = std::max(num_slots, size_t{1});  // at least one slot is required
  const auto expected_size = num_entries_ / num_slots,
             leftover = num_entries_ % num_slots;  // last remaining events after splitting

  // reset both slots readers and events ranges
  chains_.clear();
  ranges_.clear();
  // prepare the memory segments for all branches
  for (auto& bi : branches_)
    bi.second.addresses.resize(num_slots, nullptr);
  // add a TChain for each slot to be booked
  for (size_t i = 0; i < num_slots; ++i) {
    auto chain = std::make_unique<TChain>(source_.c_str());
    chain->ResetBit(kMustCleanup);
    for (const auto& filename : filenames_)
      chain->Add(filename.c_str());
    //chain->Show(0);
    for (auto& branch : branches_) {
      auto& branch_info = branch.second;
      if (const auto type_class = TClass::GetClass(branch_info.type.c_str()))
        chain->SetBranchAddress(
            branch_info.name.c_str(), &branch_info.addresses[i], nullptr, type_class, EDataType::kNoType_t, true);
      else {
        auto& addr = branch_info.addresses[i];
        if (!addr) {
          dangling_ptrs_.emplace_back(std::make_unique<double>());
          addr = dangling_ptrs_.rbegin()->get();
        }
        chain->SetBranchAddress(branch_info.name.c_str(), addr);
      }
      chain->AddBranchToCache(branch_info.name.c_str(), true);
    }
    //chain->SetCacheSize(100000000);
    chain->GetEntry(i * expected_size);
    chains_.emplace_back(std::move(chain));
    ranges_.emplace_back(std::make_pair(
        i * expected_size,                           // beginning of range
        (i + 1) * expected_size                      // end of range
            + (i < num_slots - 1 ? 0ull : leftover)  // last bit gets the remaining events
        ));
    current_entry_.emplace_back(0ull);
  }
}

bool k4TreeReader::initEntry(size_t slot, unsigned long long entry) {
  if (slot >= chains_.size() || !chains_.at(slot))
    throw std::runtime_error("Slot " + std::to_string(slot) + " is invalid as it exceeds the " +
                             std::to_string(chains_.size()) + " slot(s) booked.");
  if (entry >= num_entries_)
    throw std::runtime_error("Entry number is invalid and exceeds total number of entries in file: " +
                             std::to_string(slot) + " >= " + std::to_string(num_entries_) + ".");
  if (entry == 0 || entry != current_entry_.at(slot)) {
    auto ret = chains_.at(slot)->GetEntry(entry);
    current_entry_[slot] = entry;
    return ret > 0;
  }
  return false;
}

std::vector<void*> k4TreeReader::read(const std::string& name, const std::type_info& tid) {
  const auto& type = typeName(name);  // possibly throws
  const auto& req_tid = ROOT::Internal::RDF::TypeName2TypeID(type);
  if (req_tid != tid)
    throw std::runtime_error("Mismatching column type for '" + name + "': " + req_tid.name() + " != " + tid.name() +
                             "!");

  auto& bi = branches_.at(name);
  std::vector<void*> out(chains_.size(), nullptr);  // as many as slots
  for (size_t i = 0; i < chains_.size(); ++i)
    out[i] = (void*)&branches_[name].addresses[i];
  return out;
}
