
#include "k4DataSource/k4DataConverterFactory.h"
#include "k4DataSource/k4DataSource.h"

k4DataSource::k4DataSource(const std::vector<std::string>& filenames, const std::vector<std::string>& columns_list) {
  readers_.emplace_back(std::make_unique<k4TreeReader>("events", filenames));
  for (const auto& conv : k4DataConverterFactory::get().converters())
    std::cout << "... " << conv << std::endl;

  for (const auto& col : columns_list)
    addSource(col);
}

k4DataSource& k4DataSource::addSource(const std::string& source) {
  converters_.emplace_back(source);
  return *this;
}

void k4DataSource::SetNSlots(unsigned int nSlots) {
  for (auto& reader : readers_) {
    for (const auto& conv : converters_)
      reader->addConverter(conv);
    reader->setNumSlots(nSlots);
    for (const auto& br : reader->branches())
      if (std::find(column_names_.begin(), column_names_.end(), br) == column_names_.end())
        column_names_.emplace_back(br);
  }
  num_slots_ = nSlots;
}

void k4DataSource::Initialise() {}

std::vector<std::pair<unsigned long long, unsigned long long> > k4DataSource::GetEntryRanges() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  std::vector<std::pair<unsigned long long, unsigned long long> > out;
  for (const auto& reader : readers_)
    for (const auto& range : reader->ranges())
      if (std::find(out.begin(), out.end(), range) == out.end())
        out.emplace_back(range);
  return out;
}

bool k4DataSource::SetEntry(unsigned int slot, unsigned long long entry) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  for (auto& reader : readers_)
    if (!reader->initEntry(slot, entry))
      return false;
  return true;
}

k4DataSource::Record_t k4DataSource::GetColumnReadersImpl(std::string_view name, const std::type_info& tid) {
  const std::string br_name(name);
  Record_t outputs;
  for (const auto& rcd : readBranch(br_name, tid))  // possibly throw if not found
    outputs.emplace_back(rcd.get());
  return outputs;
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

const k4Record& k4DataSource::readBranch(const std::string& branch_name, const std::type_info& tid) const {
  // browse all input sources to find the branch with a given type
  for (auto& reader : readers_) {
    const auto& branches = reader->branches();
    if (std::find(branches.begin(), branches.end(), branch_name) == branches.end())
      continue;
    // input source was found, proceed with this one
    const auto& out = reader->read(branch_name, tid);
    if (out.size() != num_slots_)
      throw std::runtime_error("Expected " + std::to_string(num_slots_) + " value(s) and retrieved " +
                               std::to_string(out.size()) + " for branch '" + branch_name + "'.");
    return out;
  }
  throw std::runtime_error("Failed to read branch name '" + branch_name + "' from readers!");
}

k4DataFrameHandler MakeK4DataFrame(const std::vector<std::string>& file_names,
                                   const std::vector<std::string>& column_names) {
  auto ds = std::make_unique<k4DataSource>(file_names, column_names);
  ROOT::RDataFrame df(std::move(ds));
  return k4DataFrameHandler(std::move(df));
}
