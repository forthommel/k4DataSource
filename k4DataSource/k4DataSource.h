#ifndef k4DataSource_k4DataSource_h
#define k4DataSource_k4DataSource_h

#include <memory>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDataSource.hxx"

class k4DataSourceIndex {};

class k4DataSource final : public ROOT::RDF::RDataSource {
public:
  explicit k4DataSource(std::unique_ptr<RDataSource>);

  template <typename T>
  k4DataSource& addSource(const T&, const std::string& source) {
    column_names_.emplace_back(source);
    return *this;
  }

  const std::vector<std::string>& GetColumnNames() const override { return column_names_; }
  std::vector<std::pair<unsigned long long, unsigned long long> > GetEntryRanges() override;
  std::string GetTypeName(std::string_view) const override;
  bool HasColumn(std::string_view) const override;
  bool SetEntry(unsigned int, unsigned long long) override;
  void SetNSlots(unsigned int) override;

private:
  Record_t GetColumnReadersImpl(std::string_view name, const std::type_info&) override;

  RDataSource* source_{nullptr};
  std::vector<std::string> column_names_;
};

#endif
