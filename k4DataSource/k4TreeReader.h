#ifndef k4DataSource_k4TreeReader_h
#define k4DataSource_k4TreeReader_h

#include "k4DataSource/k4SlotReader.h"

class k4TreeReader {
public:
  explicit k4TreeReader(const std::string&, const std::vector<std::string>&);

  bool has(const std::string&) const;
  inline void addConverter(const std::string& converter) { converters_.emplace_back(converter); }
  inline const std::vector<std::string>& branches() const { return branches_; }
  const std::string& typeName(const std::string&) const;

  void setNumSlots(size_t);
  /// Initialise the readout of branches
  /// \param[in] slot unique slot identifier
  /// \param[in] entry event identifier
  bool initEntry(size_t slot, unsigned long long entry);
  std::vector<k4SlotReader::EventRange> ranges() const;
  const std::vector<k4Record>& read(const std::string&, const std::type_info&);

private:
  const std::string source_{};
  const std::vector<std::string> filenames_{};
  std::vector<std::string> branches_;
  std::vector<std::string> converters_;
  unsigned long long num_entries_{0ull};
  size_t current_slot_{0};

  /// List of slots booked for the input trees chain readout
  std::vector<k4SlotReader> slots_;
  std::vector<k4Record> records_;
};

#endif
