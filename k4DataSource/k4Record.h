#ifndef k4DataSource_k4Record_h
#define k4DataSource_k4Record_h

#include <memory>

class k4Record : public std::shared_ptr<void> {
public:
  k4Record() : std::shared_ptr<void>(nullptr, [](void*) {}) {}
  k4Record(void* ptr) : std::shared_ptr<void>(ptr, [](void*) {}) {}
  template <typename T>
  k4Record(const std::shared_ptr<T>& ptr) : std::shared_ptr<void>(ptr.get(), [](void*) {}), size_(sizeof(T)) {}
  template <typename T>
  k4Record(T* ptr) : std::shared_ptr<void>(ptr, [](void*) {}), size_(sizeof(T)) {}

  template <typename T>
  T* getAs() {
    return static_cast<T*>(get());
  }
  template <typename T>
  const T* getAs() const {
    return static_cast<const T*>(get());
  }
  template <typename T>
  const T& operator*() const {
    return *static_cast<T*>(get());
  }

  template <typename T>
  const k4Record& fill(T& out) const {
    out = operator*<T>();
    return *this;
  }

private:
  size_t size_{0};
};

#endif
