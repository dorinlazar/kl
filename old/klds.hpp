#pragma once

#include <kl/ds/array.hpp>
#include <kl/ds/list.hpp>
#include <kl/ds/dict.hpp>
#include <kl/ds/set.hpp>

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
