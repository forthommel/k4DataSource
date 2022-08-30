#ifndef k4DataSource_DataFormatter_h
#define k4DataSource_DataFormatter_h

#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ROOT {
  class RDataFrame;
}

class DataFormatter {
public:
  explicit DataFormatter(const std::vector<std::string>& columns_out = {});
  virtual ~DataFormatter() = default;

  const std::vector<std::string>& inputs() const { return cols_in_; }
  const std::vector<std::string>& outputs() const { return cols_out_; }

  void feed(const std::vector<void*>& input);
  std::vector<void*> extract() const;

  /// A collection translation unit
  template <typename T>
  class Handle {
  public:
    Handle() = default;
    Handle(T* ptr) : ptr_((void*)ptr) {}
    Handle(const Handle<T>& oth) : ptr_(oth.ptr_) {}
    ~Handle() = default;

    const T* operator->() const { return static_cast<T*>(ptr_); }
    const T& operator*() const { return *static_cast<T*>(ptr_); }

  private:
    void* ptr_;
  };

  /// Declare an input collection to be consumed by the algorithm
  template <typename T>
  Handle<T> consumes(const std::string& label) {
    cols_in_.emplace_back(label);
    input_data_[label] = new T;
    input_size_[label] = sizeof(T);
    return static_cast<T*>(input_data_[label]);
  }

  template <typename T>
  void produces(const std::string& label) {
    cols_out_.emplace_back(label);
    output_coll_[label] = new T;
    output_size_[label] = sizeof(T);
    output_type_[label] = typeid(T).hash_code();
  }

  virtual void convert() = 0;

  template <typename T>
  void put(std::unique_ptr<T> coll, const std::string& label = "") {
    put(coll.get(), label);
  }

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

  std::unordered_map<std::string, void*> input_data_;
  std::unordered_map<std::string, size_t> input_size_;

  std::unordered_map<std::string, void*> output_coll_;
  std::unordered_map<std::string, size_t> output_size_;
  std::unordered_map<std::string, size_t> output_type_;
};

#endif
