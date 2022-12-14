#ifndef k4DataSource_k4TreeReader_h
#define k4DataSource_k4TreeReader_h

#include "k4DataSource/k4SlotReader.h"

class k4TreeReader {
public:
  explicit k4TreeReader(const std::string&, const std::vector<std::string>&);

  using EventRange = std::pair<unsigned long long, unsigned long long>;

  bool has(const std::string&) const;
  inline void addConverter(const k4Parameters& converter) { converters_.emplace_back(converter); }
  inline const std::vector<std::string>& branches() const { return branches_; }
  const std::string& typeName(const std::string&) const;

  void setNumSlots(size_t);
  void initSlot(size_t, unsigned long long);
  /// Initialise the readout of branches
  /// \param[in] slot unique slot identifier
  /// \param[in] entry event identifier
  bool initEntry(size_t slot, unsigned long long entry);
  const std::vector<EventRange>& ranges() const { return ranges_; }
  std::vector<void*> read(const std::string&, const std::type_info&) const;

private:
  const std::string source_{};
  const std::vector<std::string> filenames_{};
  std::vector<std::string> branches_;
  std::vector<k4Parameters> converters_;
  unsigned long long num_entries_{0ull};

  /// List of slots booked for the input trees chain readout
  std::vector<k4SlotReader> slots_;
  std::vector<EventRange> ranges_;
  std::vector<void*> records_;
};

#endif
