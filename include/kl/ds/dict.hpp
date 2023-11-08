#pragma once
#include <kl/ds/list.hpp>
#include <utility>
#include <map>
#include <kl/klexcept.hpp>

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
      throw Exception("Invalid key");
    }
    return it->second;
  }

  const V& operator[](const K& key) const {
    auto it = m_map.find(key);
    if (it == m_map.end()) {
      throw Exception("Invalid key");
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

} // namespace kl
