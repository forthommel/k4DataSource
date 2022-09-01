#ifndef k4DataSource_k4SlotReader_h
#define k4DataSource_k4SlotReader_h

#include <unordered_map>
#include <vector>

#include "k4DataSource/k4Record.h"

class TChain;
class k4DataConverter;

/// Chain readout and metadata container for a readout slot
class k4SlotReader {
public:
  using EventRange = std::pair<unsigned long long, unsigned long long>;

  explicit k4SlotReader(const std::string&,
                        const std::vector<std::string>&,
                        const std::vector<std::string>&,
                        const EventRange&);

  /// Metadata and memory accessor for all branches
  struct BranchInfo {
    std::string name;    ///< Branch name
    std::string type;    ///< Human-readable branch content type
    k4Record addresses;  ///< List of addresses (1 per slot)
  };

  /// List of branch names
  std::vector<std::string> branches() const;
  const BranchInfo& branchInfo(const std::string&) const;

  bool initEntry(unsigned long long);
  const k4Record& read(const std::string&, const std::type_info&) const;
  const EventRange& range() const { return range_; }

private:
  const EventRange range_;                                ///< Range of event numbers
  std::unique_ptr<TChain> chain_;                         ///< Readout chain for tree collection
  std::unordered_map<std::string, BranchInfo> branches_;  ///< Branches memory booking and metadata
  unsigned long long current_entry_{0ull};                ///< Current event being analysed
  /// Data collections converters
  std::unordered_map<std::string, std::unique_ptr<k4DataConverter> > converters_;
  /// Collection of undocumented (= floats) addresses
  std::vector<std::unique_ptr<double> > dangling_ptrs_;
};

#endif
