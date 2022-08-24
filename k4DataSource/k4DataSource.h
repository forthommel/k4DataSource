#ifndef k4DataSource_k4DataSource_h
#define k4DataSource_k4DataSource_h

#include <memory>
#include <unordered_map>

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDataSource.hxx"

class k4DataSourceIndex {};

class k4DataSourceItem {
public:
  explicit k4DataSourceItem(const std::string& name, void* obj = nullptr) : name_(name), object_(obj) {}

  const std::string& name() const { return name_; }
  template <typename T>
  const T* get() const {
    return dynamic_cast<const T*>(object_);
  }

private:
  const std::string name_;
  void* object_;
};

class k4DataSource final : public ROOT::RDF::RDataSource {
public:
  explicit k4DataSource(std::string_view);

  template <typename T>
  k4DataSource& addSource(const std::string& source) {
    column_names_.emplace_back(source);
    column_types_.insert(std::make_pair(source, k4DataSourceItem(source, new T())));
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

  std::unique_ptr<TFile> source_;
  std::vector<std::string> column_names_;
  std::unordered_map<std::string, k4DataSourceItem> column_types_;
};

#endif
