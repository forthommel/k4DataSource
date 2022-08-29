#include <iostream>

#include "k4DataSource/k4DataConverters.h"
#include "k4DataSource/k4DataSource.h"

k4DataSource::k4DataSource(std::string_view tree_name,
                           const std::vector<std::string>& filenames,
                           const std::vector<std::string>& columns_list) {
  readers_.emplace_back(std::make_unique<k4TreeReader>(std::string(tree_name), filenames));
  for (const auto& conv : k4DataConverters::get().converters())
    std::cout << "... " << conv << std::endl;

  for (const auto& col : columns_list)
    addSource(col);
}

k4DataSource& k4DataSource::addSource(const std::string& source) {
  auto converter = k4DataConverters::get().build(source);
  column_names_.emplace_back(source);
  column_types_.insert(std::make_pair(source, k4DataSourceItem(source, std::move(converter))));
  std::cout << ">>> added " << source << std::endl;
  return *this;
}

std::vector<std::pair<unsigned long long, unsigned long long> > k4DataSource::GetEntryRanges() { return {}; }

void k4DataSource::Initialise() {
  for (auto& reader : readers_)
    reader->init();
}

bool k4DataSource::SetEntry(unsigned int slot, unsigned long long entry) {
  for (auto& reader : readers_)
    reader->initEntry(slot, entry);
  return true;
}

void k4DataSource::SetNSlots(unsigned int nSlots) {
  for (auto& reader : readers_)
    reader->setNumSlots(nSlots);
}

k4DataSource::Record_t k4DataSource::GetColumnReadersImpl(std::string_view name, const std::type_info& tid) {
  const std::string br_name(name);
  for (auto& reader : readers_) {
    const auto& branches = reader->branches();
    if (std::find(branches.begin(), branches.end(), br_name) != branches.end())
      return reader->read(br_name, tid);
  }
  //column_types_.at(source).apply(*source_);
  throw std::runtime_error("Failed to retrieve branch name '" + br_name + "' from readers!");
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
                                                     const std::vector<std::string>& file_names,
                                                     const std::vector<std::string>& column_names) {
  return ROOT::RDataFrame(std::make_unique<k4DataSource>(ntuple_name, file_names, column_names));
}
