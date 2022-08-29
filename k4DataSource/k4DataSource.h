#ifndef k4DataSource_k4DataSource_h
#define k4DataSource_k4DataSource_h

#include <memory>
#include <unordered_map>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDataSource.hxx"
#include "k4DataSource/DataFormatter.h"

class k4DataSourceItem {
public:
  explicit k4DataSourceItem(const std::string& name, std::unique_ptr<DataFormatter> converter)
      : name_(name), converter_(std::move(converter)) {}

  const std::string& name() const { return name_; }
  void apply(ROOT::RDataFrame& rdf) { converter_->convert(rdf); }

private:
  const std::string name_;
  std::unique_ptr<DataFormatter> converter_;
};

class k4DataSource final : public ROOT::RDF::RDataSource {
public:
  explicit k4DataSource(std::string_view, std::string_view, const std::vector<std::string>& = {});

  k4DataSource& addSource(const std::string&, std::unique_ptr<DataFormatter>);

  const std::vector<std::string>& GetColumnNames() const override { return column_names_; }
  std::vector<std::pair<unsigned long long, unsigned long long> > GetEntryRanges() override;
  std::string GetTypeName(std::string_view) const override;
  bool HasColumn(std::string_view) const override;
  bool SetEntry(unsigned int, unsigned long long) override;
  void SetNSlots(unsigned int) override;

private:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info&) override;

  std::unique_ptr<ROOT::RDataFrame> source_;
  std::vector<std::string> column_names_;
  std::unordered_map<std::string, k4DataSourceItem> column_types_;
};

namespace ROOT::Experimental {
  ROOT::RDataFrame MakeK4DataFrame(std::string_view ntuple_name,
                                   std::string_view file_namei,
                                   const std::vector<std::string>& = {});
}

#endif
