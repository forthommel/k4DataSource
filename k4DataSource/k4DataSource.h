#ifndef k4DataSource_k4DataSource_h
#define k4DataSource_k4DataSource_h

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RDataSource.hxx>
#include <unordered_map>

#include "k4DataSource/k4Parameters.h"
#include "k4DataSource/k4TreeReader.h"

class k4DataConverter;

/// A ROOT RDataSource-derived high level reader
class k4DataSource final : public ROOT::RDF::RDataSource {
public:
  explicit k4DataSource(const std::vector<std::string>&, const std::vector<std::string>& = {});
  explicit k4DataSource(const std::vector<std::string>&, const std::vector<k4DataConverter>&);

  k4DataSource& addSource(const k4Parameters&);

  bool HasColumn(std::string_view) const override;
  const std::vector<std::string>& GetColumnNames() const override { return column_names_; }
  std::string GetTypeName(std::string_view) const override;

  void SetNSlots(unsigned int) override;
  void InitSlot(unsigned int, unsigned long long) override;
  void Initialise() override;
  std::vector<std::pair<unsigned long long, unsigned long long> > GetEntryRanges() override;
  bool SetEntry(unsigned int, unsigned long long) override;

private:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info&) override;

  size_t num_slots_{1};
  bool retrieved_ranges_{false};
  std::vector<k4Parameters> converters_;
  std::vector<std::unique_ptr<k4TreeReader> > readers_;

  // output source-oriented information
  std::vector<std::string> column_names_;
};

/// A helper object to expose the data source to the outside world
class k4DataFrameHandler {
public:
  k4DataFrameHandler(ROOT::RDataFrame&& rdf) : rdf_(rdf) {}

  ROOT::RDataFrame* operator->() { return &rdf_; }

private:
  ROOT::RDataFrame rdf_;
};

k4DataFrameHandler MakeK4DataFrame(const std::vector<std::string>& file_names, const std::vector<std::string>& = {});
k4DataFrameHandler MakeK4DataFrame(const std::vector<std::string>& file_names, const std::vector<k4DataConverter>&);

#endif
