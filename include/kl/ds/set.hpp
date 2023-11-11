#pragma once

#include <set>
#include <initializer_list>

#include <kl/ds/list.hpp>

namespace kl {
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
} // namespace kl