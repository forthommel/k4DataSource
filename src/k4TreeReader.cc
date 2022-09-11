#include <TChain.h>

#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4Logger.h"
#include "k4DataSource/k4Parameters.h"
#include "k4DataSource/k4TreeReader.h"

k4TreeReader::k4TreeReader(const std::string& source, const std::vector<std::string>& filenames)
    : source_(source), filenames_(filenames) {
  TChain chain_model(source.c_str());
  for (const auto& filename : filenames_)
    chain_model.Add(filename.c_str());
  num_entries_ = chain_model.GetEntries();
}

bool k4TreeReader::has(const std::string& name) const {
  return std::find(branches_.begin(), branches_.end(), name) != branches_.end();
}

std::string k4TreeReader::typeName(const std::string& name) const {
  if (!has(name))
    throw k4Error << "Input tree does not have branch name '" << name << "'!";
  if (slots_.empty())
    throw k4Error << "Branch type name search requested while number of slots is not yet initialised.";
  return slots_.at(0).branchInfo(name).type->GetName();
}

void k4TreeReader::setNumSlots(size_t num_slots) {
  if (num_slots < 1)
    throw k4Error << "Invalid number of slots: " << num_slots << ".";  // at least one slot is required
  slots_.clear();
  const auto expected_size = num_entries_ / num_slots,
             leftover = num_entries_ % num_slots;  // last remaining events after splitting
  for (size_t i = 0; i < num_slots; ++i) {
    slots_.emplace_back(source_, filenames_, converters_);
    ranges_.emplace_back(std::make_pair(
        i * expected_size,                           // beginning of range
        (i + 1) * expected_size                      // end of range
            + (i < num_slots - 1 ? 0ull : leftover)  // last bit gets the remaining events
        ));
  }
  branches_ = slots_[0].branches();
}

void k4TreeReader::initSlot(size_t slot, unsigned long long entry) {
  if (slot >= slots_.size())
    throw k4Error << "Invalid slot index requested:\n"
                  << "  maximal value: " << (slots_.size() - 1) << ",\n"
                  << "  got: " << slot << ".";
  if (!branches_.empty() && slots_[slot].branches().size() != branches_.size())
    throw k4Error << "Slot " << slot << " created with different number of branches than reference! "
                  << "This may be the sign of a nasty memory corruption...";
  branches_ = slots_[slot].branches();
  slots_.at(slot).initEntry(entry);
}

bool k4TreeReader::initEntry(size_t slot, unsigned long long entry) {
  if (slot >= slots_.size())
    throw k4Error << "Invalid slot index requested:\n"
                  << "  maximal value: " << (slots_.size() - 1) << ",\n"
                  << "  got: " << slot << ".";
  if (entry >= num_entries_)
    throw k4Error << "Invalid entry requested:\n"
                  << "  maximal value: " << (num_entries_ - 1) << ",\n"
                  << "  got: " << slot << ".";
  return slots_.at(slot).initEntry(entry);
}

std::vector<void*> k4TreeReader::read(const std::string& name, const std::type_info& tid) const {
  std::vector<void*> output;
  for (size_t i = 0; i < slots_.size(); ++i) {
    const auto& vals = slots_.at(i).read(name, tid);
    if (!vals)
      throw k4Error << "Failed to retrieve value of slot " << i << " for variable name '" << name << "'.";
    output.emplace_back(vals.value().data);
  }
  return output;
}
