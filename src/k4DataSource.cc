//#include "edm4hep/ReconstructedParticleData.h"
#include "k4DataSource/k4DataConverters.h"
#include "k4DataSource/k4DataSource.h"

k4DataSource::k4DataSource(std::string_view source, std::string_view filename)
    : source_(new ROOT::RDataFrame(source, filename)) {
  //addSource<edm4hep::ReconstructedParticleData>("ReconstructedParticles", {});
  for (const auto& nm : source_->GetColumnNames())
    std::cout << ">>>> " << nm << std::endl;
  for (const auto& conv : k4DataConverters::get().converters())
    std::cout << "... " << conv << std::endl;
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

ROOT::RDataFrame ROOT::Experimental::MakeK4DataFrame(std::string_view ntuple_name, std::string_view file_name) {
  return ROOT::RDataFrame(std::make_unique<k4DataSource>(ntuple_name, file_name));
}
