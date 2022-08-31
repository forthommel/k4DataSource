
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
  auto converter = k4DataConverterFactory::get().build(source);
  //for (const auto& output : converter->outputs())
  //  column_names_.emplace_back(output);
  for (const auto& in_coll : converter->inputs())
    converter->setInputType(in_coll, TClass::GetClass(GetTypeName(in_coll).c_str()));
  for (const auto& out_coll : converter->outputs())
    converter->setOutputType(out_coll, TClass::GetClass(GetTypeName(out_coll).c_str()));
  column_names_.emplace_back(source);
  column_types_.insert(std::make_pair(source, k4DataSourceItem(source, std::move(converter))));
  std::cout << ">>> added " << source << std::endl;
  return *this;
}

void k4DataSource::SetNSlots(unsigned int nSlots) {
  for (auto& reader : readers_)
    reader->setNumSlots(nSlots);
  num_slots_ = nSlots;
}

void k4DataSource::Initialise() {}

std::vector<std::pair<unsigned long long, unsigned long long> > k4DataSource::GetEntryRanges() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  std::vector<std::pair<unsigned long long, unsigned long long> > out;
  for (const auto& reader : readers_)
    for (const auto& range : reader->ranges())
      out.emplace_back(range);  //FIXME
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
  // first browse the list of conversion modules loaded in runtime
  for (auto& col : column_types_) {
    if (col.first != name)
      continue;
    const auto& mod_inputs = col.second.converter().inputs();
    col.second.converter().describe();
    // found corresponding module ; will start conversion of inputs
    std::vector<std::vector<k4Handle> > inputs(num_slots_,  // one per slot
                                               std::vector<k4Handle>(mod_inputs.size(), nullptr));
    for (size_t i = 0; i < mod_inputs.size(); ++i) {
      const auto& input = mod_inputs.at(i);
      // read input branch, and return a vector of contents (one per slot)
      const auto& var_content = readBranch(input, ROOT::Internal::RDF::TypeName2TypeID(GetTypeName(input)));
      for (size_t j = 0; j < num_slots_; ++j) {
        inputs[j][i] = var_content.at(j);
      }
    }
    for (const auto& input : inputs)  // one per slot
      outputs.emplace_back(col.second.apply(input).at(0).get());
    //FIXME also use other output collections if available
    return outputs;
  }
  // did not find in modules ; will look into the input file branches
  for (const auto& rcd : readBranch(br_name, tid))  // possibly throw if not found
    outputs.emplace_back(rcd.get());
  return outputs;
}

bool k4DataSource::HasColumn(std::string_view col_name) const {
  for (const auto& col : column_names_)
    if (col == col_name)
      return true;
  for (const auto& reader : readers_)
    if (reader->has(std::string(col_name)))
      return true;
  return false;
}

std::string k4DataSource::GetTypeName(std::string_view type) const {
  // first browse the columns build from a conversion module
  for (const auto& col : column_types_)
    if (col.first == type) {
      const auto& outputs = col.second.converter().outputs();
      if (outputs.size() == 1)
        return outputs.at(0);  //FIXME only supported mode for now
      throw std::runtime_error("Unsupported output format providing " + std::to_string(outputs.size()) +
                               " collections.");
    }
  // then browse the input source columns
  std::string stype(type);
  for (const auto& reader : readers_)
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
