#ifndef k4DataSource_k4DataConverter_h
#define k4DataSource_k4DataConverter_h

#include <cstring>
#include <map>
#include <string>

#include "k4DataSource/k4Record.h"

class TClass;

/// A base algorithm for the production of event collections
class k4DataConverter {
public:
  k4DataConverter();
  virtual ~k4DataConverter() = default;

  /// User-defined collection building
  virtual void convert() = 0;

  /// A collection translation unit
  template <typename T>
  class Handle : public std::shared_ptr<T> {
  public:
    using std::shared_ptr<T>::shared_ptr;
    explicit Handle(k4Handle& ptr) : std::shared_ptr<T>(ptr.getAs<T>()) {}
    virtual ~Handle() = default;
  };

  /// Feed the algorithm a set of input values
  void feed(const std::vector<k4Handle>&);
  /// Declare an input collection to be consumed by the algorithm
  template <typename T>
  Handle<T> consumes(const std::string& label) {
    cols_in_.emplace_back(label);
    inputs_.insert(std::make_pair(label, Collection{new T(), typeid(T), sizeof(T), typeid(T).hash_code()}));
    return Handle<T>(inputs_[label].collection);
  }
  /// Retrieve a list of input collections consumed by this module
  const std::vector<std::string>& inputs() const { return cols_in_; }
  const std::type_info& inputType(const std::string& coll) const { return inputs_.at(coll).type_info; }

  /// Extract all collections produced by the algorithm
  std::vector<k4Handle> extract() const;
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
    if (label.empty()) {
      for (const auto& output : outputs_) {
        if (output.second.type == typeid(T).hash_code())
          ptr = output.second.collection;
      }
      if (!ptr)
        return;
    } else
      ptr = outputs_.at(label).collection;
    memcpy(ptr.get(), coll, sizeof(T));
  }

  void describe() const;

private:
  std::vector<std::string> cols_in_;
  std::vector<std::string> cols_out_;

  struct Collection {
    const TClass* classType() const;
    k4Handle collection{nullptr};
    const std::type_info& type_info{typeid(int)};
    size_t size{0ull};
    size_t type{0ull};
  };
  std::map<std::string, Collection> inputs_;
  std::map<std::string, Collection> outputs_;
};

#endif
