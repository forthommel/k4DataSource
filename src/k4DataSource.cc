//#include "edm4hep/ReconstructedParticleData.h"
#include "k4DataSource/k4DataConverters.h"
#include "k4DataSource/k4DataSource.h"

k4DataSource::k4DataSource(std::string_view source,
                           std::string_view filename,
                           const std::vector<std::string>& columns_list)
    : source_(new ROOT::RDataFrame(source, filename)) {
  //addSource<edm4hep::ReconstructedParticleData>("ReconstructedParticles", {});
  for (const auto& nm : source_->GetColumnNames())
    std::cout << ">>>> " << nm << std::endl;
  for (const auto& conv : k4DataConverters::get().converters())
    std::cout << "... " << conv << std::endl;

  for (const auto& col : columns_list) {
    addSource(col, std::move(k4DataConverters::get().build(col)));
    std::cout << ">>> added " << col << std::endl;
  }
  for (const auto& nm : source_->GetColumnNames())
    std::cout << "new>>>> " << nm << std::endl;
}

k4DataSource& k4DataSource::addSource(const std::string& source, std::unique_ptr<DataFormatter> converter) {
  column_names_.emplace_back(source);
  column_types_.insert(std::make_pair(source, k4DataSourceItem(source, std::move(converter))));
  column_types_.at(source).apply(*source_);
  return *this;
}

std::vector<std::pair<unsigned long long, unsigned long long> > k4DataSource::GetEntryRanges() { return {}; }

bool k4DataSource::SetEntry(unsigned int slot, unsigned long long entry) {
  (void)slot;
  (void)entry;
  return true;
}

void k4DataSource::SetNSlots(unsigned int nSlots) { (void)nSlots; }

k4DataSource::Record_t k4DataSource::GetColumnReadersImpl(std::string_view name, const std::type_info&) {
  (void)name;
  return Record_t{};
}

bool k4DataSource::HasColumn(std::string_view col_name) const {
  for (const auto& col : column_names_)
    if (col == col_name)
      return true;
  return false;
}

std::string k4DataSource::GetTypeName(std::string_view type) const {
  for (const auto& col : column_types_)
    if (col.first == type)
      return col.second.name();
  return "";
}

ROOT::RDataFrame ROOT::Experimental::MakeK4DataFrame(std::string_view ntuple_name,
                                                     std::string_view file_name,
                                                     const std::vector<std::string>& column_names) {
  return ROOT::RDataFrame(std::make_unique<k4DataSource>(ntuple_name, file_name, column_names));
}
