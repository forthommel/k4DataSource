#ifndef k4DataSource_k4Parameters_h
#define k4DataSource_k4Parameters_h

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#define DEFINE_TYPE(type)                                                      \
  template <>                                                                  \
  bool k4Parameters::has<type>(const std::string& key) const;                  \
  template <>                                                                  \
  type k4Parameters::get<type>(const std::string& key, const type& def) const; \
  template <>                                                                  \
  type& k4Parameters::operator[]<type>(const std::string& key);                \
  template <>                                                                  \
  k4Parameters& k4Parameters::set<type>(const std::string& key, const type&);  \
  template <>                                                                  \
  std::vector<std::string> k4Parameters::keysOf<type>() const;

template <typename T>
std::string merge(const std::vector<T>& in, const std::string& delim = ",");
template <typename T>
std::string merge(const std::vector<std::vector<T> >& in, const std::string& delim = ",");
std::vector<std::string> split(const std::string&, char delim = ',');

/// Parameters container
class k4Parameters {
private:
  /// Retrieve the default argument for a given variable type
  template <typename T>
  struct default_arg {
    /// Default variable argument
    static T get() { return T(); }
  };

public:
  k4Parameters() = default;
  /// Copy constructor
  k4Parameters(const k4Parameters&);
  ~k4Parameters() {}  // required for unique_ptr initialisation! avoids cleaning all individual objects
  k4Parameters& operator=(const k4Parameters&) = default;  ///< Assignment operator

  /// Equality operator
  bool operator==(const k4Parameters&) const;
  /// Inequality operator
  bool operator!=(const k4Parameters& oth) const { return !operator==(oth); }

  /// Check if a given parameter is handled in this list
  template <typename T>
  bool has(const std::string& key) const;
  /// Erase a parameter with key
  /// \return Number of key-indexed values erased
  size_t erase(const std::string&);
  /// Retrieve the module name if any
  template <typename T>
  inline T name(const T& def = default_arg<T>::get()) const {
    if (!has<T>(MODULE_NAME))
      return def;
    return get<T>(MODULE_NAME);
  }
  /// Set the module name
  template <typename T>
  inline k4Parameters& setName(const T& value) {
    return set<T>(MODULE_NAME, value);
  }
  /// Fill a variable with the key content if exists
  template <typename T>
  const k4Parameters& fill(const std::string& key, T& value) const {
    if (has<T>(key))
      value = get<T>(key);
    return *this;
  }
  /// Get a parameter value
  template <typename T>
  T get(const std::string& key, const T& def = default_arg<T>::get()) const;
  /// Get a recasted parameter value
  template <typename T, typename U>
  inline U getAs(const std::string& key, const U& def = default_arg<U>::get()) const {
    return static_cast<U>(get<T>(key, static_cast<T>(def)));
  }
  /// Reference to a parameter value
  template <typename T>
  T& operator[](const std::string& key);
  /// Set a parameter value
  template <typename T>
  k4Parameters& set(const std::string& key, const T& value);
  /// Set a recasted parameter value
  template <typename T, typename U>
  inline k4Parameters& setAs(const std::string& key, const U& value) {
    return set<T>(key, static_cast<T>(value));
  }
  /// Rename the key to a parameter value
  k4Parameters& rename(const std::string& old_key, const std::string& new_key);
  /// Concatenate two parameters containers
  k4Parameters& operator+=(const k4Parameters& oth);
  /// Concatenation of two parameters containers
  k4Parameters operator+(const k4Parameters& oth) const;
  /// Is the list empty?
  bool empty() const;

  /// List of keys for one type in this list of parameters
  template <typename T>
  std::vector<std::string> keysOf() const;
  /// List of keys handled in this list of parameters
  /// \param[in] name_key Include the name variable?
  std::vector<std::string> keys(bool name_key = true) const;
  /// Get a string-converted version of a value
  /// \param[in] wrap Encapsulate the value with type()
  std::string getString(const std::string& key, bool wrap = false) const;
  /// Serialise a parameters collection into a parseable string
  std::string serialise() const;

  /// Human-readable version of a parameters container
  friend std::ostream& operator<<(std::ostream& os, const k4Parameters&);
  /// Debugging-like printout of a parameters container
  const k4Parameters& print(std::ostream&) const;
  /// Indexing key for the module name
  static const std::string MODULE_NAME;

private:
  std::map<std::string, k4Parameters> param_values_;
  std::unordered_map<std::string, bool> bool_values_;
  std::unordered_map<std::string, int> int_values_;
  std::unordered_map<std::string, unsigned long long> ulong_values_;
  std::unordered_map<std::string, double> dbl_values_;
  std::unordered_map<std::string, std::string> str_values_;
  std::unordered_map<std::string, std::vector<int> > vec_int_values_;
  std::unordered_map<std::string, std::vector<double> > vec_dbl_values_;
  std::unordered_map<std::string, std::vector<std::string> > vec_str_values_;
  std::unordered_map<std::string, std::vector<k4Parameters> > vec_param_values_;
  std::unordered_map<std::string, std::vector<std::vector<double> > > vec_vec_dbl_values_;
};

DEFINE_TYPE(k4Parameters)
DEFINE_TYPE(bool)
DEFINE_TYPE(int)
DEFINE_TYPE(unsigned long long)
DEFINE_TYPE(double)
DEFINE_TYPE(std::string)
DEFINE_TYPE(std::vector<int>)
DEFINE_TYPE(std::vector<double>)
DEFINE_TYPE(std::vector<std::string>)
DEFINE_TYPE(std::vector<k4Parameters>)
DEFINE_TYPE(std::vector<std::vector<double> >)

#undef DEFINE_TYPE
#endif
