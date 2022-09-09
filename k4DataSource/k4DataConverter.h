#ifndef k4DataSource_k4DataConverter_h
#define k4DataSource_k4DataConverter_h

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "k4DataSource/k4Parameters.h"
#include "podio/CollectionBuffers.h"

class TClass;

/// A base algorithm for the production of event collections
class k4DataConverter {
public:
  explicit k4DataConverter(const k4Parameters&);

  const k4Parameters& parameters() const { return params_; }
  /// User-defined collection building
  virtual void convert() {}

protected:
  void throwFailedToConsume(const std::type_info&, const std::string&) const;
  void throwFailedToPut(const std::type_info&, const std::string&) const;

  /// A collection translation unit
  template <typename T>
  class Handle : public std::unique_ptr<T> {
  public:
    using std::unique_ptr<T>::unique_ptr;
    explicit Handle(void*& ptr) : std::unique_ptr<T>(static_cast<T*>(ptr)) {}
    virtual ~Handle() { std::unique_ptr<T>::release(); }
  };

public:
  const std::string& name() const { return conv_name_; }

  /// Feed the algorithm a set of input values
  void feed(const std::vector<podio::CollectionReadBuffers>&);
  /// Declare an input collection to be consumed by the algorithm
  template <typename T>
  Handle<T> consumes(const std::string& label) {
    cols_in_.emplace_back(label);
    if (!inputs_
             .insert(std::make_pair(
                 label, Collection{podio::CollectionWriteBuffers{new T(), nullptr, nullptr}, typeid(T), sizeof(T)}))
             .second)
      throwFailedToConsume(typeid(T), label);
    return Handle<T>(inputs_[label].collection.data);
  }
  /// Retrieve a list of input collections consumed by this module
  const std::vector<std::string>& inputs() const { return cols_in_; }
  const std::type_info& inputType(const std::string& coll) const { return inputs_.at(coll).type_info; }

  /// Extract all collections produced by the algorithm
  std::unordered_map<std::string, podio::CollectionWriteBuffers> extract() const;
  /// Declare an output collection to be produced by the algorithm
  template <typename T>
  void produces(const std::string& label = "") {
    const auto conv_label = (label.empty() ? conv_name_ : label);
    cols_out_.emplace_back(conv_label);
    outputs_.insert(std::make_pair(
        conv_label, Collection{podio::CollectionWriteBuffers{new T(), nullptr, nullptr}, typeid(T), sizeof(T)}));
  }
  /// Retrieve a list of output collections provided by this module
  const std::vector<std::string>& outputs() const { return cols_out_; }
  const std::type_info& outputType(const std::string& coll) const { return outputs_.at(coll).type_info; }
  podio::CollectionWriteBuffers output(const std::string& coll) const { return outputs_.at(coll).collection; }

  /// Put the collection onto the event
  template <typename T>
  void put(std::unique_ptr<T> coll, const std::string& label = "") {
    if (!label.empty()) {
      if (outputs_.count(label) == 0)
        throwFailedToPut(typeid(T), label);
      *static_cast<T*>(outputs_.at(label).collection.data) = *coll;
      return;
    }
    for (auto& output : outputs_)
      if (output.second.type_info == typeid(T)) {
        *static_cast<T*>(output.second.collection.data) = *coll;
        return;
      }
    throwFailedToPut(typeid(T), label);
  }

  void describe() const;

private:
  const std::string conv_name_;
  const k4Parameters params_;
  std::vector<std::string> cols_in_;
  std::vector<std::string> cols_out_;

  struct Collection {
    const TClass* classType() const;
    podio::CollectionWriteBuffers collection{};
    const std::type_info& type_info{typeid(int)};
    size_t size{0ull};
  };
  std::unordered_map<std::string, Collection> inputs_;
  std::unordered_map<std::string, Collection> outputs_;
};

#endif
