#pragma once

#include <map>
#include <set>
#include <queue>
#include <exception>
#include <stdexcept>
#include <functional>
#include <optional>
#include <memory>
// TODO(dorin): switch to ranges.
namespace kl {

template <typename T>
using ptr = std::shared_ptr<T>;

template <typename T>
using uptr = std::unique_ptr<T>;

template <typename T>
class List {
  std::vector<T> m_vec;

public:
  List() = default;
  List(std::initializer_list<T> v) : m_vec(v) {}
  explicit List(size_t size) { m_vec.reserve(size); }
  void clear() { m_vec.clear(); }

  bool operator==(const List<T>& l) const {
    if (&l == this) {
      return true;
    }
    return m_vec == l.m_vec;
  }

  [[nodiscard]] T& operator[](size_t pos) {
    if (pos >= m_vec.size()) {
      throw std::out_of_range("Invalid index access");
    }
    return m_vec[pos];
  }
  [[nodiscard]] const T& operator[](size_t pos) const {
    if (pos >= m_vec.size()) {
      throw std::out_of_range("Invalid index access");
    }
    return m_vec[pos];
  }

  [[nodiscard]] auto begin() { return m_vec.begin(); }
  [[nodiscard]] auto end() { return m_vec.end(); }
  [[nodiscard]] auto begin() const { return m_vec.begin(); }
  [[nodiscard]] auto end() const { return m_vec.end(); }

  [[nodiscard]] size_t size() const { return m_vec.size(); }

  void add(T&& value) { m_vec.emplace_back(std::move(value)); }
  void add(const T& value) { m_vec.push_back(value); }
  void add(const List<T>& other) {
    m_vec.reserve(size() + other.size());
    for (const auto& t: other) {
      m_vec.push_back(t);
    }
  }
  void add(std::initializer_list<T> other) {
    m_vec.reserve(size() + other.size());
    for (const auto& t: other) {
      m_vec.push_back(t);
    }
  }

  [[nodiscard]] bool has(const T& value) const { return m_vec.contains(value); }

  void for_each(std::function<void(const T&)> op) const {
    for (const auto& item: m_vec) {
      op(item);
    }
  }

  template <typename U>
  List<U> transform(std::function<U(const T&)> tr) const {
    List<U> res(size());
    for (const auto& item: m_vec) {
      res.add(tr(item));
    }
    return res;
  }

  List<T>& sort_in_place() {
    std::sort(m_vec.begin(), m_vec.end());
    return *this;
  }

  // TODO(dorin) UTs
  void remove(const T& value) { m_vec.erase(std::remove(m_vec.begin(), m_vec.end(), value), m_vec.end()); }
  void remove_at(size_t index) {
    if (index < size()) {
      auto it = m_vec.begin() + index;
      m_vec.erase(it, it + 1);
    }
  }
  void remove_range(size_t index, size_t rangeSize) {
    if (index < size() && rangeSize > 0) {
      auto it = m_vec.begin() + index;
      auto offset = index + rangeSize;
      auto endit = offset >= m_vec.size() ? m_vec.end() : m_vec.begin() + offset;
      m_vec.erase(it, endit);
    }
  }
  bool all(std::function<bool(const T&)> op) const { return std::all_of(m_vec.begin(), m_vec.end(), op); }
  bool any(std::function<bool(const T&)> op) const { return std::any_of(m_vec.begin(), m_vec.end(), op); }
  List<T> select(std::function<bool(const T&)> op) const {
    List<T> res;
    for (const auto& item: m_vec) {
      if (op(item)) {
        res.add(item);
      }
    }
    return res;
  }
  T& last() { return m_vec.back(); }
};

template <typename K, typename V>
class Dict {
  std::map<K, V> m_map;

public:
  Dict() = default;
  explicit Dict(const List<std::pair<K, V>>& pairList) {
    m_map.reserve(pairList.size());
    for (const auto& it: pairList) {
      add(it.first(), it.second());
    }
  }

  void clear() { m_map.clear(); }

  auto begin() const { return m_map.cbegin(); }
  auto end() const { return m_map.cend(); }
  auto begin() { return m_map.begin(); }
  auto end() { return m_map.end(); }

  [[nodiscard]] size_t size() const { return m_map.size(); }

  V& operator[](const K& key) {
    auto it = m_map.find(key);
    if (it == m_map.end()) {
      throw std::out_of_range("Invalid key");
    }
    return it->second;
  }

  const V& operator[](const K& key) const {
    auto it = m_map.find(key);
    if (it == m_map.end()) {
      throw std::out_of_range("Invalid key");
    }
    return it->second;
  }

  const V& get(const K& key, const V& default_value = V()) const {
    auto it = m_map.find(key);
    if (it == m_map.end()) {
      return default_value;
    }
    return it->second;
  }

  std::optional<V> get_opt(const K& key) const {
    auto it = m_map.find(key);
    if (it == m_map.end()) {
      return {};
    }
    return it->second;
  }

  void add(const K& key, V&& value) { m_map.insert_or_assign(key, std::move(value)); }
  void add(const K& key, const V& value) { m_map.insert_or_assign(key, value); }

  void remove(const K& key) { m_map.erase(key); }

  bool has(const K& key) const { return m_map.contains(key); }

  List<K> keys() const {
    List<K> list(m_map.size());
    for (const auto& [k, v]: m_map) {
      list.add(k);
    }
    return list;
  }
  List<V> values() const {
    List<V> list(m_map.size());
    for (const auto& [k, v]: m_map) {
      list.add(v);
    }
    return list;
  }
};

template <typename T1, typename T2>
class List<std::pair<T1, T2>> {
public:
  Dict<T1, T2> to_dict() { return Dict(*this); }
};

template <typename T>
class Set {
  std::set<T> m_data;

public:
  Set() = default;
  Set(std::initializer_list<T> v) : m_data(v) {}

  auto begin() const { return m_data.begin(); }
  auto end() const { return m_data.end(); }
  auto begin() { return m_data.begin(); }
  auto end() { return m_data.end(); }

  [[nodiscard]] size_t size() const { return m_data.size(); }

  void add(T&& value) { m_data.insert(std::move(value)); }
  void add(const T& value) { m_data.insert(value); }
  void add(const List<T>& other) {
    for (const auto& t: other) {
      m_data.insert(t);
    }
  }
  void add(const Set<T>& other) {
    for (const auto& t: other) {
      m_data.insert(t);
    }
  }

  void remove(const T& value) { m_data.erase(value); }

  bool has(const T& v) { return m_data.contains(v); }

  void for_each(std::function<void(const T&)> op) const {
    for (const auto& item: m_data) {
      op(item);
    }
  }

  List<T> to_list() const {
    List<T> lst(size());
    for (const auto& item: m_data) {
      lst.add(item);
    }
    return lst;
  }
};

template <typename T>
using PList = List<uptr<T>>;

} // namespace kl

template <typename K, typename V>
inline std::ostream& operator<<(std::ostream& os, const kl::Dict<K, V>& d) {
  os << "{";
  bool comma_needed = false;
  for (const auto& [k, v]: d) {
    if (comma_needed) {
      os << ",";
    } else {
      comma_needed = true;
    }
    os << k << ":" << v;
  }
  return os << "}";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const kl::List<T>& l) {
  os << "[";
  bool comma_needed = false;
  for (const auto& t: l) {
    if (comma_needed) {
      os << ",";
    } else {
      comma_needed = true;
    }
    os << t;
  }
  return os << "]";
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const kl::Set<T>& l) {
  os << "[";
  bool comma_needed = false;
  for (const auto& t: l) {
    if (comma_needed) {
      os << ",";
    } else {
      comma_needed = true;
    }
    os << t;
  }
  return os << "]";
}
