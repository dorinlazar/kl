#pragma once

#include <kl/kl.hpp>
#include <vector>
#include <kl/klexcept.hpp>
#include <kl/ds/cursor.hpp>

namespace kl {

// TODO:
// - Use `deducing this` to eliminate redundat implementation
// - Implement cursors.
// - Implement own replacement for std::vector with copy-on-write strategy

template <typename T>
class List {
  std::vector<T> m_vec;

public:
  List() = default;
  List(std::initializer_list<T> v) : m_vec(v) {}
  explicit List(size_t size) { m_vec.reserve(size); }
  void clear() { m_vec.clear(); }

  bool operator==(const List<T>& l) const = default;

  [[nodiscard]] auto& operator[](size_t pos) {
    if (pos >= m_vec.size()) {
      throw RuntimeError("Invalid index access");
    }
    return m_vec[pos];
  }
  [[nodiscard]] const auto& operator[](size_t pos) const {
    if (pos >= m_vec.size()) {
      throw RuntimeError("Invalid index access");
    }
    return m_vec[pos];
  }

  [[nodiscard]] auto begin() { return m_vec.begin(); }
  [[nodiscard]] auto begin() const { return m_vec.begin(); }
  [[nodiscard]] auto end() { return m_vec.end(); }
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

public: // enumerable flow functionality
  LinearCursor flow_start() const { return {0uz}; }
  bool flow_is_valid(LinearCursor cursor) const { return cursor.pos() < m_vec.size(); }
  const T& flow_at(LinearCursor cursor) const& {
    if (flow_is_valid(cursor)) [[likely]] {
      return m_vec[cursor.pos()];
    }
    throw RuntimeError("Invalid index access");
  }
  T& flow_at(LinearCursor cursor) & {
    if (flow_is_valid(cursor)) [[likely]] {
      return m_vec[cursor.pos()];
    }
    throw RuntimeError("Invalid index access");
  }
  void flow_advance(LinearCursor& cursor) const { cursor.inc(); }
};

template <typename T>
using PList = List<uptr<T>>;

} // namespace kl