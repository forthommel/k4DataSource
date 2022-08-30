#ifndef k4DataSource_DataFormatter_h
#define k4DataSource_DataFormatter_h

#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    input_coll_[label] = new T();
    input_size_[label] = sizeof(T);
    return Handle<T>(input_coll_[label]);
  }
  /// Retrieve a list of input collections consumed by this module
  const std::vector<std::string>& inputs() const { return cols_in_; }

  /// Extract all collections produced by the algorithm
  std::vector<void*> extract() const;
  /// Declare an output collection to be produced by the algorithm
  template <typename T>
  void produces(const std::string& label) {
    cols_out_.emplace_back(label);
    output_coll_[label] = new T;
    output_size_[label] = sizeof(T);
    output_type_[label] = typeid(T).hash_code();
  }
  /// Retrieve a list of output collections provided by this module
  const std::vector<std::string>& outputs() const { return cols_out_; }

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
      for (const auto& info : output_type_) {
        if (info.second == typeid(T).hash_code())
          ptr = output_coll_.at(info.first);
      }
      if (!ptr)
        return;
    } else
      ptr = output_coll_.at(label);
    memcpy(ptr, coll, sizeof(T));
  }

private:
  std::vector<std::string> cols_in_;
  std::vector<std::string> cols_out_;

  std::unordered_map<std::string, void*> input_coll_;
  std::unordered_map<std::string, size_t> input_size_;

  std::unordered_map<std::string, void*> output_coll_;
  std::unordered_map<std::string, size_t> output_size_;
  std::unordered_map<std::string, size_t> output_type_;
};

#endif
