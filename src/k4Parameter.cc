#include <climits>
#include <iomanip>
#include <regex>

#include "k4DataSource/k4Parameters.h"

#define IMPL_TYPE_GET(type, coll, name)                                         \
  template <>                                                                   \
  type k4Parameters::get<type>(const std::string& key, const type& def) const { \
    if (coll.count(key) > 0)                                                    \
      return coll.at(key);                                                      \
    return def;                                                                 \
  }

#define IMPL_TYPE_SET(type, coll, name)                                                                             \
  template <>                                                                                                       \
  bool k4Parameters::has<type>(const std::string& key) const {                                                      \
    return coll.count(key) != 0;                                                                                    \
  }                                                                                                                 \
  template <>                                                                                                       \
  k4Parameters& k4Parameters::set<type>(const std::string& key, const type& value) {                                \
    coll[key] = value;                                                                                              \
    return *this;                                                                                                   \
  }                                                                                                                 \
  template <>                                                                                                       \
  type& k4Parameters::operator[]<type>(const std::string& key) {                                                    \
    return coll[key];                                                                                               \
  }                                                                                                                 \
  template <>                                                                                                       \
  std::vector<std::string> k4Parameters::keysOf<type>() const {                                                     \
    std::vector<std::string> out;                                                                                   \
    std::transform(coll.begin(), coll.end(), std::back_inserter(out), [](const auto& pair) { return pair.first; }); \
    return out;                                                                                                     \
  }

#define IMPL_TYPE_ALL(type, coll, name) \
  IMPL_TYPE_GET(type, coll, #name)      \
  IMPL_TYPE_SET(type, coll, #name)

const std::string k4Parameters::MODULE_NAME = "mod_name";

k4Parameters::k4Parameters(const k4Parameters& oth) { operator+=(oth); }

bool k4Parameters::operator==(const k4Parameters& oth) const {
  if (bool_values_ != oth.bool_values_)
    return false;
  if (int_values_ != oth.int_values_)
    return false;
  if (ulong_values_ != oth.ulong_values_)
    return false;
  if (dbl_values_ != oth.dbl_values_)
    return false;
  if (str_values_ != oth.str_values_)
    return false;
  if (param_values_ != oth.param_values_)
    return false;
  if (vec_int_values_ != oth.vec_int_values_)
    return false;
  if (vec_dbl_values_ != oth.vec_dbl_values_)
    return false;
  if (vec_str_values_ != oth.vec_str_values_)
    return false;
  if (vec_param_values_ != oth.vec_param_values_)
    return false;
  return true;
}

k4Parameters& k4Parameters::operator+=(const k4Parameters& oth) {
  // ensure the two collections are not identical
  if (*this == oth)
    return *this;
  // then check if any key of the other collection is lready present in the list
  std::vector<std::string> keys_erased;
  for (const auto& key : oth.keys()) {
    if (has<k4Parameters>(key)) {
      // do not remove a duplicate parameters collection if they are not strictly identical ;
      // will concatenate its values with the other object's
      if (get<k4Parameters>(key) == oth.get<k4Parameters>(key) && erase(key) > 0)
        keys_erased.emplace_back(key);
    } else if (erase(key) > 0)
      // any other duplicate key is just replaced
      keys_erased.emplace_back(key);
  }
  //--- concatenate all typed lists
  bool_values_.insert(oth.bool_values_.begin(), oth.bool_values_.end());
  int_values_.insert(oth.int_values_.begin(), oth.int_values_.end());
  ulong_values_.insert(oth.ulong_values_.begin(), oth.ulong_values_.end());
  dbl_values_.insert(oth.dbl_values_.begin(), oth.dbl_values_.end());
  str_values_.insert(oth.str_values_.begin(), oth.str_values_.end());
  // special case for parameters collection: concatenate values instead of full containers
  for (const auto& par : oth.param_values_)
    // if the two parameters list are modules, and do not have the same name,
    // simply replace the old one with the new parameters list
    if (param_values_[par.first].getString(k4Parameters::MODULE_NAME) ==
        par.second.getString(k4Parameters::MODULE_NAME))
      param_values_[par.first] += par.second;
    else
      param_values_[par.first] = par.second;
  vec_int_values_.insert(oth.vec_int_values_.begin(), oth.vec_int_values_.end());
  vec_dbl_values_.insert(oth.vec_dbl_values_.begin(), oth.vec_dbl_values_.end());
  vec_str_values_.insert(oth.vec_str_values_.begin(), oth.vec_str_values_.end());
  vec_param_values_.insert(oth.vec_param_values_.begin(), oth.vec_param_values_.end());
  vec_vec_dbl_values_.insert(oth.vec_vec_dbl_values_.begin(), oth.vec_vec_dbl_values_.end());
  return *this;
}

k4Parameters k4Parameters::operator+(const k4Parameters& oth) const {
  k4Parameters out = *this;
  out += oth;
  return out;
}

size_t k4Parameters::erase(const std::string& key) {
  size_t out = 0ull;
  if (bool_values_.count(key) > 0)
    out += bool_values_.erase(key);
  if (int_values_.count(key) > 0)
    out += int_values_.erase(key);
  if (ulong_values_.count(key) > 0)
    out += ulong_values_.erase(key);
  if (dbl_values_.count(key) > 0)
    out += dbl_values_.erase(key);
  if (str_values_.count(key) > 0)
    out += str_values_.erase(key);
  if (param_values_.count(key) > 0)
    out += param_values_.erase(key);
  if (vec_int_values_.count(key) > 0)
    out += vec_int_values_.erase(key);
  if (vec_dbl_values_.count(key) > 0)
    out += vec_dbl_values_.erase(key);
  if (vec_str_values_.count(key) > 0)
    out += vec_str_values_.erase(key);
  if (vec_param_values_.count(key) > 0)
    out += vec_param_values_.erase(key);
  if (vec_vec_dbl_values_.count(key) != 0)
    out += vec_vec_dbl_values_.erase(key);
  return out;
}

bool k4Parameters::empty() const { return keys(true).empty(); }

std::ostream& operator<<(std::ostream& os, const k4Parameters& params) {
  params.print(os);
  return os;
}

const k4Parameters& k4Parameters::print(std::ostream& os) const {
  if (empty()) {
    os << "{}";
    return *this;
  }
  std::string sep;
  const auto& plist_name = getString(k4Parameters::MODULE_NAME);
  if (!plist_name.empty()) {
    auto mod_name = has<std::string>(MODULE_NAME) ? "\"" + plist_name + "\"" : plist_name;
    os << "Module(" << mod_name, sep = ", ";
  } else
    os << "Parameters(";
  for (const auto& key : keys(false))
    os << sep << key << "=" << getString(key, true), sep = ", ";
  os << ")";
  return *this;
}

std::vector<std::string> k4Parameters::keys(bool name_key) const {
  std::vector<std::string> out{};
  auto key = [](const auto& p) { return p.first; };
  std::transform(bool_values_.begin(), bool_values_.end(), std::back_inserter(out), key);
  std::transform(int_values_.begin(), int_values_.end(), std::back_inserter(out), key);
  std::transform(ulong_values_.begin(), ulong_values_.end(), std::back_inserter(out), key);
  std::transform(dbl_values_.begin(), dbl_values_.end(), std::back_inserter(out), key);
  std::transform(str_values_.begin(), str_values_.end(), std::back_inserter(out), key);
  std::transform(param_values_.begin(), param_values_.end(), std::back_inserter(out), key);
  std::transform(vec_int_values_.begin(), vec_int_values_.end(), std::back_inserter(out), key);
  std::transform(vec_dbl_values_.begin(), vec_dbl_values_.end(), std::back_inserter(out), key);
  std::transform(vec_str_values_.begin(), vec_str_values_.end(), std::back_inserter(out), key);
  std::transform(vec_param_values_.begin(), vec_param_values_.end(), std::back_inserter(out), key);
  std::transform(vec_vec_dbl_values_.begin(), vec_vec_dbl_values_.end(), std::back_inserter(out), key);
  if (!name_key) {
    const auto it_name = std::find(out.begin(), out.end(), MODULE_NAME);
    if (it_name != out.end())
      out.erase(it_name);
  }
  std::sort(out.begin(), out.end());
  return out;
}

std::string k4Parameters::getString(const std::string& key, bool wrap) const {
  auto wrap_val = [&wrap](const auto& val, const std::string& type) -> std::string {
    std::ostringstream os;
    if (type == "float" || type == "vfloat")
      os << std::fixed;
    else if (type == "bool")
      os << std::boolalpha;
    os << val;
    return (wrap ? type + "(" : "")  //+ (type == "bool" ? utils::yesno(std::stoi(os.str())) : os.str()) +
           + os.str() + (wrap ? ")" : "");
  };
  auto wrap_coll = [&wrap_val](const auto& coll, const std::string& type) -> std::string {
    return wrap_val(merge(coll, ", "), type);
  };
  std::ostringstream os;
  if (has<k4Parameters>(key))
    os << get<k4Parameters>(key);
  else if (has<bool>(key))
    os << wrap_val(get<bool>(key), "bool");
  else if (has<int>(key))
    os << wrap_val(get<int>(key), "int");
  else if (has<unsigned long long>(key))
    os << wrap_val(get<unsigned long long>(key), "ulong");
  else if (has<double>(key))
    os << wrap_val(get<double>(key), "float");
  else if (has<std::string>(key))
    os << wrap_val(get<std::string>(key), "str");
  else if (has<std::vector<k4Parameters> >(key))
    os << wrap_coll(get<std::vector<k4Parameters> >(key), "VParams");
  else if (has<std::vector<int> >(key))
    os << wrap_coll(get<std::vector<int> >(key), "vint");
  else if (has<std::vector<std::string> >(key))
    os << wrap_coll(get<std::vector<std::string> >(key), "vstr");
  else if (has<std::vector<std::vector<double> > >(key))
    os << wrap_coll(get<std::vector<std::vector<double> > >(key), "vvfloat");
  return os.str();
}

k4Parameters& k4Parameters::rename(const std::string& old_key, const std::string& new_key) {
  if (has<bool>(old_key))
    set(new_key, get<bool>(old_key)).erase(old_key);
  if (has<int>(old_key))
    set(new_key, get<int>(old_key)).erase(old_key);
  if (has<unsigned long long>(old_key))
    set(new_key, get<unsigned long long>(old_key)).erase(old_key);
  if (has<double>(old_key))
    set(new_key, get<double>(old_key)).erase(old_key);
  if (has<std::string>(old_key))
    set(new_key, get<std::string>(old_key)).erase(old_key);
  if (has<k4Parameters>(old_key))
    set(new_key, get<k4Parameters>(old_key)).erase(old_key);
  if (has<std::vector<int> >(old_key))
    set(new_key, get<std::vector<int> >(old_key)).erase(old_key);
  if (has<std::vector<double> >(old_key))
    set(new_key, get<std::vector<double> >(old_key)).erase(old_key);
  if (has<std::vector<std::string> >(old_key))
    set(new_key, get<std::vector<std::string> >(old_key)).erase(old_key);
  if (has<std::vector<k4Parameters> >(old_key))
    set(new_key, get<std::vector<k4Parameters> >(old_key)).erase(old_key);
  return *this;
}

std::string k4Parameters::serialise() const {
  std::ostringstream out;
  std::string sep;
  for (const auto& key : keys(true)) {
    out << sep << key;
    if (has<k4Parameters>(key)) {
      const auto& plist = get<k4Parameters>(key);
      out << "/";
      if (plist.keys().size() > 1)
        out << "{";
      out << plist.serialise();
      if (plist.keys().size() > 1)
        out << "}";
    } else
      out << "=" << getString(key, false);
    sep = ",";
  }
  return out.str();
}

//------------------------------------------------------------------
// default template (placeholders)
//------------------------------------------------------------------

template <typename T>
bool k4Parameters::has(const std::string& key) const {
  throw std::runtime_error("Invalid type for key=" + key + "!");
}

template <typename T>
T k4Parameters::get(const std::string& key, const T&) const {
  throw std::runtime_error("Invalid type retrieved for key=" + key + "!");
}

template <typename T>
T& k4Parameters::operator[](const std::string& key) {
  throw std::runtime_error("Invalid type retrieved for key=" + key + "!");
}

template <typename T>
k4Parameters& k4Parameters::set(const std::string& key, const T&) {
  throw std::runtime_error("Invalid type to be set for key=" + key + "!");
}

//------------------------------------------------------------------
// sub-parameters-type attributes
//------------------------------------------------------------------

IMPL_TYPE_ALL(k4Parameters, param_values_, "parameters")
IMPL_TYPE_ALL(bool, bool_values_, "boolean")

IMPL_TYPE_SET(int, int_values_, "integer")
template <>
int k4Parameters::get<int>(const std::string& key, const int& def) const {
  if (has<int>(key))
    return int_values_.at(key);
  if (has<unsigned long long>(key)) {
    const auto ulong_val = ulong_values_.at(key);
    //if (ulong_val >= INT_MAX)
    //  CG_WARNING("k4Parameters:get")
    //      << "Trying to retrieve a (too) long unsigned integer with an integer getter. Please fix your code.";
    return (int)ulong_val;
  }
  return def;
}

IMPL_TYPE_SET(unsigned long long, ulong_values_, "unsigned long integer")
template <>
unsigned long long k4Parameters::get<unsigned long long>(const std::string& key, const unsigned long long& def) const {
  if (has<unsigned long long>(key))
    return ulong_values_.at(key);
  if (has<int>(key)) {
    const auto& int_val = int_values_.at(key);
    if (int_val < 0)
      //CG_WARNING("k4Parameters:get")
      //    << "Trying to retrieve a negative-value integer with an unsigned long getter. Please fix your code.";
      return int_val;
  }
  return def;
}

IMPL_TYPE_ALL(double, dbl_values_, "floating number")
IMPL_TYPE_ALL(std::string, str_values_, "string")
IMPL_TYPE_ALL(std::vector<int>, vec_int_values_, "vector of integers")
IMPL_TYPE_ALL(std::vector<double>, vec_dbl_values_, "vector of floating numbers")
IMPL_TYPE_ALL(std::vector<std::string>, vec_str_values_, "vector of strings")
IMPL_TYPE_ALL(std::vector<k4Parameters>, vec_param_values_, "vector of parameters")
IMPL_TYPE_ALL(std::vector<std::vector<double> >, vec_vec_dbl_values_, "vector of vectors of floating numbers")

#undef IMPL_TYPE

template <typename T>
std::string merge(const std::vector<T>& in, const std::string& delim) {
  std::string out;
  for (size_t i = 0; i < in.size(); ++i)
    out += (i > 0 ? delim : "") + std::to_string(in.at(i));
  return out;
}

template <>
std::string merge(const std::vector<std::string>& in, const std::string& delim) {
  std::string out;
  for (size_t i = 0; i < in.size(); ++i)
    out += (i > 0 ? delim : "") + in.at(i);
  return out;
}

template <typename T>
std::string merge(const std::vector<std::vector<T> >& in, const std::string& delim) {
  std::string out;
  for (size_t i = 0; i < in.size(); ++i)
    out += (i > 0 ? delim : "") + merge(in.at(i));
  return out;
}

template <>
std::string merge(const std::vector<k4Parameters>& in, const std::string& delim) {
  std::string out;
  for (size_t i = 0; i < in.size(); ++i)
    out += (i > 0 ? delim : "") + in.at(i).serialise();
  return out;
}
