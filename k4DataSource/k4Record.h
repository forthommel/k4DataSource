#ifndef k4DataSource_k4Record_h
#define k4DataSource_k4Record_h

#include <memory>
#include <vector>

class k4Handle : public std::shared_ptr<void> {
public:
  using std::shared_ptr<void>::shared_ptr;

  template <typename T>
  k4Handle(const shared_ptr<T>& ptr) : std::shared_ptr<void>(ptr.get()), size_(sizeof(T)) {}
  template <typename T>
  k4Handle(T* ptr) : std::shared_ptr<void>(ptr, [](void*) {}), size_(sizeof(T)) {}

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
  const k4Handle& operator>>(T& out) const {
    out = operator*<T>();
    return *this;
  }

private:
  size_t size_{0};
};

/// A collection of data records, one per slot
struct k4Record : public std::vector<k4Handle> {
  using std::vector<k4Handle>::vector;

  template <typename T>
  std::vector<T*> as() const;
};

#endif
