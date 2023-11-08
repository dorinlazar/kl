#pragma once

#include <map>
#include <set>
#include <queue>
#include <exception>
#include <stdexcept>
#include <functional>
#include <optional>
#include <kl/ds/list.hpp>

namespace kl {
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
