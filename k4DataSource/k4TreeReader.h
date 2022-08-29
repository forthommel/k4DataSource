#ifndef k4DataSource_k4TreeReader_h
#define k4DataSource_k4TreeReader_h

#include <TChain.h>

#include <memory>

class k4TreeReader {
public:
  explicit k4TreeReader(const std::string&, const std::vector<std::string>&);

  bool has(const std::string&) const;
  const std::vector<std::string>& branches() const { return branch_names_; }
  const std::string& typeName(const std::string&) const;

  void setNumSlots(size_t);
  void init();
  /// Initialise the readout of branches
  /// \param[in] slot unique slot identifier
  /// \param[in] entry event identifier
  bool initEntry(size_t slot, unsigned long long entry);
  inline const std::vector<std::pair<unsigned long long, unsigned long long> >& ranges() const { return ranges_; }
  std::vector<void*> read(const std::string&, const std::type_info&);

private:
  const std::string source_{};
  const std::vector<std::string> filenames_{};
  unsigned long long num_entries_{0ull};

  /// Metadata and memory accessor for all branches
  struct BranchInfo {
    std::string name;              ///< Branch name
    std::string type;              ///< Human-readable branch content type
    std::vector<void*> addresses;  ///< List of addresses (1 per slot)
  };
  // input ROOT TTree-oriented information
  std::vector<std::string> branch_names_;
  std::unordered_map<std::string, BranchInfo> branches_;

  std::vector<std::unique_ptr<TChain> > chains_;                            // one chain per slot
  std::vector<std::pair<unsigned long long, unsigned long long> > ranges_;  // one range per slot

  std::vector<std::unique_ptr<double> > dangling_ptrs_;
};

#endif
