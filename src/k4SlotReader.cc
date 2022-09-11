#include <ROOT/RDF/Utils.hxx>

#include "k4DataSource/k4DataConverter.h"
#include "k4DataSource/k4DataConverterFactory.h"
#include "k4DataSource/k4Logger.h"
#include "k4DataSource/k4Parameters.h"
#include "k4DataSource/k4SlotReader.h"

k4SlotReader::k4SlotReader(const std::string& source,
                           const std::vector<std::string>& filenames,
                           const std::vector<k4Parameters>& converters)
    : source_(source), reader_(new podio::ROOTFrameReader) {
  if (filenames.empty())
    throw k4Error << "At least one filename should be provided.";
  {
    // define a temporary instance to read branches names
    auto frame_reader = std::make_unique<podio::ROOTFrameReader>();
    frame_reader->openFiles(filenames);
    auto entry_data = std::move(frame_reader->readNextEntry(source_));
    auto classes_data = frame_reader->storedClasses(source_);
    for (const auto& cat : entry_data->getAvailableCollections()) {
      if (classes_data.count(cat) == 0) {
        k4Log << "Failed to retrieve class data for collection '" << cat << "'.";
        continue;
      }
      auto* class_info = std::get<0>(classes_data.at(cat));
      if (!class_info) {
        k4Log << "Failed to retrieve TClass object for collection '" << cat << "'.";
        continue;
      } else if (class_info->GetName())
        class_info = TClass::GetClass(class_info->GetName());
      if (!class_info->GetTypeInfo()) {
        k4Log << "Failed to retrieve type info for collection '" << cat << "' with ROOT type '" << class_info->GetName()
              << "'.";
        continue;
      }
      auto buff = entry_data->getCollectionBuffers(cat);
      branches_.insert(std::make_pair(cat, BranchInfo{true, cat, class_info, {}}));
      k4Log << "Inserted a new branch with name '" << cat << "' and type '" << class_info->GetName()
            << "' from podio ROOT frame reader.";
    }
  }
  reader_->openFiles(filenames);
  entry_data_ = std::move(reader_->readNextEntry(source_));
  /*for (const auto& converter : converters) {
    auto conv = k4DataConverterFactory::get().build(converter);
    for (const auto& out_coll : conv->outputs())
      branches_.insert(std::make_pair(out_coll,
                                      BranchInfo{false,
                                                 out_coll,
                                                 ROOT::Internal::RDF::TypeID2TypeName(conv->outputType(out_coll)),
                                                 conv->output(out_coll)}));
    conv->describe();
    converters_[converter.name<std::string>()] = std::move(conv);
  }*/
}

std::vector<std::string> k4SlotReader::branches() const {
  std::vector<std::string> out;
  for (const auto& branch : branches_)
    out.emplace_back(branch.first);
  for (const auto& conv : converters_)
    for (const auto& out_coll : conv.second->outputs())
      out.emplace_back(out_coll);
  return out;
}

const k4SlotReader::BranchInfo& k4SlotReader::branchInfo(const std::string& branch) const {
  if (branches_.count(branch) == 0)
    throw k4Error << "Failed to retrieve info about branch with name '" << branch << "'.";
  return branches_.at(branch);
}

bool k4SlotReader::initEntry(unsigned long long event) {
  if (event == current_event_)
    return true;
  current_event_ = event;
  /*for (auto& col : converters_) {
    auto& conv = col.second;
    // prepare all input collections for the converter
    std::vector<void*> inputs;
    for (const auto& input : conv->inputs()) {
      const auto& info = branchInfo(input);
      inputs.emplace_back(info.in_tree ? info.address                         // first browse the input tree branches
                                       : read(input, conv->inputType(input))  // then check the converters outputs
      );
    }
    // launch the conversion
    conv->feed(inputs);
    conv->convert();
  }
  return ret > 0;*/
  entry_data_ = std::move(reader_->readNextEntry(source_));
  if (!entry_data_) {
    k4Log << "Failed to read next entry";
    return false;
  }
  return true;
}

std::optional<podio::CollectionReadBuffers> k4SlotReader::read(const std::string& name,
                                                               const std::type_info& tid) const {
  k4Log << __PRETTY_FUNCTION__;
  if (branches_.count(name) == 0)
    throw k4Error << "Failed to retrieve column '" << name
                  << "', neither in conversion modules outputs nor in input tree columns.";
  const auto* type = branches_.at(name).type;
  if (&tid != type->GetTypeInfo())
    throw k4Error << "Invalid type requested for column '" << name << "':\n"
                  << "  expected " << type->GetName() << ",\n"
                  << "  got " << tid.name() << ".";
  // first loop over the various converters and spot if one produces the column name
  for (auto& col : converters_) {
    auto& conv = col.second;
    const auto& mod_outputs = conv->outputs();
    if (std::find(mod_outputs.begin(), mod_outputs.end(), name) == mod_outputs.end())
      continue;
    // found corresponding module ; return a conversion of inputs
    return conv->extract().at(name);
  }
  if (!entry_data_)
    throw k4Error << "Failed to retrieve entry data for column '" << name << "'.";
  // return the value in the input trees
  return entry_data_->getCollectionBuffers(name);
}
