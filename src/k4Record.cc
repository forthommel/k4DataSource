#include "k4DataSource/k4Record.h"

template <typename T>
std::vector<T*> k4Record::as() const {
  std::vector<T*> out(size(), new char[sizeof(T)]);
  for (auto i = 0; i < size(); ++i)
    at(i) >> out.at(i);
  return out;
}
