#ifndef k4DataSource_k4SlotReader_h
#define k4DataSource_k4SlotReader_h

#include <memory>
#include <unordered_map>
#include <vector>

class TChain;
class k4DataConverter;
class k4Parameters;

/// Chain readout and metadata container for a readout slot
class k4SlotReader {
public:
  using EventRange = std::pair<unsigned long long, unsigned long long>;

  explicit k4SlotReader(const std::string&,
                        const std::vector<std::string>&,
                        const std::vector<k4Parameters>&,
                        const EventRange&);

  /// Metadata and memory accessor for all branches
  struct BranchInfo {
    bool in_tree{false};     ///< Is the branch collected from the tree or produced by a converter?
    std::string name;        ///< Branch name
    std::string type;        ///< Human-readable branch content type
    void* address{nullptr};  ///< Branch address
  };

  /// List of branch names
  std::vector<std::string> branches() const;
  const BranchInfo& branchInfo(const std::string&) const;

  bool initEntry(unsigned long long);
  void* read(const std::string&, const std::type_info&) const;
  const EventRange& range() const { return range_; }

private:
  const EventRange range_;                                ///< Range of event numbers
  std::unique_ptr<TChain> chain_;                         ///< Readout chain for tree collection
  std::unordered_map<std::string, BranchInfo> branches_;  ///< Branches memory booking and metadata
  /// Data collections converters
  std::unordered_map<std::string, std::unique_ptr<k4DataConverter> > converters_;
  unsigned long long current_event_{0ull};  ///< Current event being analysed
};

#endif
