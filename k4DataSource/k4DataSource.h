#ifndef k4DataSource_k4DataSource_h
#define k4DataSource_k4DataSource_h

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDataSource.hxx>
#include <unordered_map>

#include "k4DataSource/k4DataSourceItem.h"
#include "k4DataSource/k4TreeReader.h"

/// A ROOT RDataSource-derived high level reader
class k4DataSource final : public ROOT::RDF::RDataSource {
public:
  explicit k4DataSource(std::string_view, const std::vector<std::string>&, const std::vector<std::string>& = {});

  k4DataSource& addSource(const std::string&);

  bool HasColumn(std::string_view) const override;
  const std::vector<std::string>& GetColumnNames() const override { return column_names_; }
  std::string GetTypeName(std::string_view) const override;

  void SetNSlots(unsigned int) override;
  void Initialise() override;
  std::vector<std::pair<unsigned long long, unsigned long long> > GetEntryRanges() override;
  bool SetEntry(unsigned int, unsigned long long) override;

private:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info&) override;
  std::vector<void*> readBranch(const std::string&, const std::type_info&);

  size_t num_slots_{1};
  std::vector<std::unique_ptr<k4TreeReader> > readers_;

  // output source-oriented information
  std::vector<std::string> column_names_;
  std::unordered_map<std::string, k4DataSourceItem> column_types_;
};

namespace ROOT::Experimental {
  ROOT::RDataFrame MakeK4DataFrame(std::string_view ntuple_name,
                                   const std::vector<std::string>& file_names,
                                   const std::vector<std::string>& = {});
}

#endif
