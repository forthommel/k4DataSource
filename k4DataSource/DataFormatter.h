#ifndef k4DataSource_DataFormatter_h
#define k4DataSource_DataFormatter_h

#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

class TClass;

/// A base algorithm for the production of event collections
class DataFormatter {
public:
  DataFormatter() = default;
  virtual ~DataFormatter() = default;

  /// User-defined collection building
  virtual void convert() = 0;

  /// A collection translation unit
  template <typename T>
  class Handle : public std::shared_ptr<T> {
  public:
    Handle() = default;
    explicit Handle(void* ptr) : std::shared_ptr<T>(new (ptr) T()) {}
    virtual ~Handle() = default;
  };

  /// Feed the algorithm a set of input values
  void feed(const std::vector<void*>&);
  /// Declare an input collection to be consumed by the algorithm
  template <typename T>
  Handle<T> consumes(const std::string& label) {
    cols_in_.emplace_back(label);
    inputs_[label] = Collection{new T(), nullptr, sizeof(T), typeid(T).hash_code()};
    return Handle<T>(inputs_[label].collection);
  }
  /// Retrieve a list of input collections consumed by this module
  const std::vector<std::string>& inputs() const { return cols_in_; }
  void setInputType(const std::string& coll, TClass* type) { inputs_.at(coll).class_type = type; }

  /// Extract all collections produced by the algorithm
  std::vector<void*> extract() const;
  /// Declare an output collection to be produced by the algorithm
  template <typename T>
  void produces(const std::string& label) {
    cols_out_.emplace_back(label);
    outputs_[label] = Collection{new T(), nullptr, sizeof(T), typeid(T).hash_code()};
  }
  /// Retrieve a list of output collections provided by this module
  const std::vector<std::string>& outputs() const { return cols_out_; }
  void setOutputType(const std::string& coll, TClass* type) { outputs_.at(coll).class_type = type; }

  /// Put the collection onto the event
  template <typename T>
  void put(std::unique_ptr<T> coll, const std::string& label = "") {
    put(coll.get(), label);
  }
  /// Put the collection onto the event
  template <typename T>
  void put(const T* coll, const std::string& label = "") {
    void* ptr{nullptr};
    if (label.empty()) {
      for (const auto& output : outputs_) {
        if (output.second.type == typeid(T).hash_code())
          ptr = output.second.collection;
      }
      if (!ptr)
        return;
    } else
      ptr = outputs_.at(label).collection;
    memcpy(ptr, coll, sizeof(T));
  }

  void describe() const;

private:
  std::vector<std::string> cols_in_;
  std::vector<std::string> cols_out_;

  struct Collection {
    void* collection{nullptr};
    TClass* class_type{nullptr};
    size_t size{0ull};
    size_t type{0ull};
  };
  std::map<std::string, Collection> inputs_;
  std::map<std::string, Collection> outputs_;
};

#endif
