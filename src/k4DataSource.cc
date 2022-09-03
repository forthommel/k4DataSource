#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"
#include "k4DataSource/k4DataSource.h"

k4DataSource::k4DataSource(const std::vector<std::string>& filenames, const std::vector<std::string>& columns_list) {
  readers_.emplace_back(std::make_unique<k4TreeReader>("events", filenames));
  for (const auto& conv : k4DataConverterFactory::get().converters())
    std::cout << "... " << conv << std::endl;

  for (const auto& col : columns_list)
    addSource(k4Parameters().setName(col));
}

k4DataSource::k4DataSource(const std::vector<std::string>& filenames, const std::vector<k4DataConverter>& converters) {
  readers_.emplace_back(std::make_unique<k4TreeReader>("events", filenames));
  /*for (const auto& conv : k4DataConverterFactory::get().converters())
    std::cout << "... " << conv << std::endl;

  for (const auto& col : columns_list)
    addSource(col);*/
}

k4DataSource& k4DataSource::addSource(const k4Parameters& params) {
  converters_.emplace_back(params);
  return *this;
}

void k4DataSource::SetNSlots(unsigned int num_slots) {
  for (auto& reader : readers_) {
    for (const auto& conv : converters_)
      reader->addConverter(conv);
    reader->setNumSlots(num_slots);
    for (const auto& br : reader->branches())
      if (std::find(column_names_.begin(), column_names_.end(), br) == column_names_.end())
        column_names_.emplace_back(br);
  }
  num_slots_ = num_slots;
}

void k4DataSource::InitSlot(unsigned int slot, unsigned long long entry) {
  for (auto& reader : readers_)
    reader->initSlot(slot, entry);
}

void k4DataSource::Initialise() { retrieved_ranges_ = false; }

std::vector<std::pair<unsigned long long, unsigned long long> > k4DataSource::GetEntryRanges() {
  std::vector<std::pair<unsigned long long, unsigned long long> > ranges;
  if (retrieved_ranges_)
    return ranges;

  for (const auto& reader : readers_)
    for (const auto& range : reader->ranges())
      if (std::find(ranges.begin(), ranges.end(), range) == ranges.end())
        ranges.emplace_back(range);
  retrieved_ranges_ = true;
  return ranges;
}

bool k4DataSource::SetEntry(unsigned int slot, unsigned long long entry) {
  for (auto& reader : readers_)
    if (!reader->initEntry(slot, entry))
      return false;
  return true;
}

k4DataSource::Record_t k4DataSource::GetColumnReadersImpl(std::string_view name, const std::type_info& tid) {
  const std::string br_name(name);
  // browse all input sources to find the branch with a given type
  Record_t outputs;
  for (auto& reader : readers_) {
    const auto& branches = reader->branches();
    if (std::find(branches.begin(), branches.end(), br_name) == branches.end())
      continue;
    // input source was found, proceed with this one
    const auto& out = reader->read(br_name, tid);
    if (out.size() != num_slots_)
      throw std::runtime_error("Expected " + std::to_string(num_slots_) + " value(s) and retrieved " +
                               std::to_string(out.size()) + " for branch '" + br_name + "'.");
    return out;
  }
  throw std::runtime_error("Failed to read branch name '" + br_name + "' from readers!");
}

bool k4DataSource::HasColumn(std::string_view col_name) const {
  for (const auto& col : column_names_)
    if (col == col_name)
      return true;
  return false;
}

std::string k4DataSource::GetTypeName(std::string_view type) const {
  std::string stype(type);
  for (const auto& reader : readers_)  // browse the input source columns
    if (reader->has(stype))
      return reader->typeName(stype);
  throw std::runtime_error("Failed to retrieve a collection of type '" + stype +
                           "', neither in the input file nor in the list of converters.");
}

k4DataFrameHandler MakeK4DataFrame(const std::vector<std::string>& file_names,
                                   const std::vector<std::string>& column_names) {
  auto ds = std::make_unique<k4DataSource>(file_names, column_names);
  ROOT::RDataFrame df(std::move(ds));
  return k4DataFrameHandler(std::move(df));
}

k4DataFrameHandler MakeK4DataFrame(const std::vector<std::string>& file_names,
                                   const std::vector<k4DataConverter>& converters) {
  auto ds = std::make_unique<k4DataSource>(file_names, converters);
  ROOT::RDataFrame df(std::move(ds));
  return k4DataFrameHandler(std::move(df));
}
