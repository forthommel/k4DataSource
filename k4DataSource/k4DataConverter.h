#ifndef k4DataSource_k4DataConverter_h
#define k4DataSource_k4DataConverter_h

#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "k4DataSource/k4Record.h"

class TClass;

/// A base algorithm for the production of event collections
class k4DataConverter {
public:
  k4DataConverter();
  virtual ~k4DataConverter() = default;

  /// User-defined collection building
  virtual void convert() = 0;

protected:
  void throwFailedToConsume(const std::type_info&, const std::string&) const;
  void throwFailedToPut(const std::type_info&, const std::string&) const;

  /// A collection translation unit
  template <typename T>
  class Handle : public std::shared_ptr<T> {
  public:
    using std::shared_ptr<T>::shared_ptr;
    explicit Handle(void*& ptr) : std::shared_ptr<T>(static_cast<T*>(ptr)) {}
    virtual ~Handle() = default;
  };

public:
  /// Feed the algorithm a set of input values
  void feed(const std::vector<k4Record>&);
  /// Declare an input collection to be consumed by the algorithm
  template <typename T>
  Handle<T> consumes(const std::string& label) {
    cols_in_.emplace_back(label);
    if (!inputs_.insert(std::make_pair(label, Collection{new T(), typeid(T), sizeof(T), typeid(T).hash_code()})).second)
      throwFailedToConsume(typeid(T), label);
    return Handle<T>(inputs_[label].collection);
  }
  /// Retrieve a list of input collections consumed by this module
  const std::vector<std::string>& inputs() const { return cols_in_; }
  const std::type_info& inputType(const std::string& coll) const { return inputs_.at(coll).type_info; }

  /// Extract all collections produced by the algorithm
  std::vector<k4Record> extract() const;
  /// Declare an output collection to be produced by the algorithm
  template <typename T>
  void produces(const std::string& label) {
    cols_out_.emplace_back(label);
    outputs_.insert(std::make_pair(label, Collection{new T(), typeid(T), sizeof(T), typeid(T).hash_code()}));
  }
  /// Retrieve a list of output collections provided by this module
  const std::vector<std::string>& outputs() const { return cols_out_; }
  const std::type_info& outputType(const std::string& coll) const { return outputs_.at(coll).type_info; }

  /// Put the collection onto the event
  template <typename T>
  void put(std::unique_ptr<T> coll, const std::string& label = "") {
    put(coll.get(), label);
  }
  /// Put the collection onto the event
  template <typename T>
  void put(const T* coll, const std::string& label = "") {
    if (!label.empty()) {
      std::memcpy(outputs_.at(label).collection, coll, sizeof(T));
      return;
    }
    for (const auto& output : outputs_)
      if (output.second.type == typeid(T).hash_code()) {
        std::memcpy(output.second.collection, coll, sizeof(T));
        return;
      }
    throwFailedToPut(typeid(T), label);
  }

  void describe() const;

private:
  std::vector<std::string> cols_in_;
  std::vector<std::string> cols_out_;

  struct Collection {
    const TClass* classType() const;
    void* collection{nullptr};
    const std::type_info& type_info{typeid(int)};
    size_t size{0ull};
    size_t type{0ull};
  };
  std::unordered_map<std::string, Collection> inputs_;
  std::unordered_map<std::string, Collection> outputs_;
};

#endif
