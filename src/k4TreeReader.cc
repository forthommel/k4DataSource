#include <TChain.h>


#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4TreeReader.h"

k4TreeReader::k4TreeReader(const std::string& source, const std::vector<std::string>& filenames)
    : source_(source), filenames_(filenames) {
  TChain chain_model(source.c_str());
  for (const auto& filename : filenames_)
    chain_model.Add(filename.c_str());
  const auto* branches = chain_model.GetListOfBranches();
  for (size_t i = 0; i < branches->GetEntries(); ++i)
    branches_.emplace_back(branches->At(i)->GetName());
  num_entries_ = chain_model.GetEntries();
}

bool k4TreeReader::has(const std::string& name) const {
  return std::find(branches_.begin(), branches_.end(), name) != branches_.end();
}

std::vector<k4SlotReader::EventRange> k4TreeReader::ranges() const {
  std::vector<k4SlotReader::EventRange> out;
  for (const auto& slot : slots_)
    out.emplace_back(slot.range());
  return out;
}

const std::string& k4TreeReader::typeName(const std::string& name) const {
  if (!has(name))
    throw std::runtime_error("Input tree does not have branch name '" + name + "'!");
  if (slots_.empty())
    throw std::runtime_error("Branch type name search requested while number of slots is not yet initialised.");
  return slots_.at(0).branchInfo(name).type;
}

void k4TreeReader::setNumSlots(size_t num_slots) {
  num_slots = std::max(num_slots, size_t{1});  // at least one slot is required
  const auto expected_size = num_entries_ / num_slots,
             leftover = num_entries_ % num_slots;  // last remaining events after splitting

  // reset all readout chain and other metadata for slots list
  slots_.clear();
  records_.clear();
  // add a TChain for each slot to be booked
  for (size_t i = 0; i < num_slots; ++i) {
    auto range = std::make_pair(i * expected_size,                           // beginning of range
                                (i + 1) * expected_size                      // end of range
                                    + (i < num_slots - 1 ? 0ull : leftover)  // last bit gets the remaining events
    );
    slots_.emplace_back(source_, filenames_, converters_, range);
    records_.emplace_back();
    if (i == 0)
      branches_ = slots_.back().branches();
    else if (slots_.back().branches().size() != branches_.size())
      throw std::runtime_error("Slot " + std::to_string(i) +
                               " created with different number of branches than reference! This may be the sign of a "
                               "nasty memory corruption...");
  }
}

bool k4TreeReader::initEntry(size_t slot, unsigned long long entry) {
  if (slot >= slots_.size())
    throw std::runtime_error("Invalid slot index requested:\n  maximal value: " + std::to_string(slots_.size() - 1) +
                             ",\n  got: " + std::to_string(slot) + ".");
  if (entry >= num_entries_)
    throw std::runtime_error("Invalid entry requested:\n  maximal value: " + std::to_string(num_entries_ - 1) +
                             ",\n  got: " + std::to_string(slot) + ".");
  current_slot_ = slot;
  return slots_.at(slot).initEntry(entry);
}

const std::vector<k4Record>& k4TreeReader::read(const std::string& name, const std::type_info& tid) {
  if (current_slot_ >= slots_.size())
    throw std::runtime_error("Invalid slot index requested:\n  maximal value: " + std::to_string(slots_.size() - 1) +
                             ",\n  got: " + std::to_string(current_slot_) + ".");
  for (size_t i = 0; i < slots_.size(); ++i)
    records_.at(i) = slots_.at(i).read(name, tid);
  return records_;
}
