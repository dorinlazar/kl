#pragma once

#include <variant>

#include "klds.hpp"
#include "kltext.hpp"

namespace kl {

enum class ValueType : int { Null = 0, Scalar = 1, Map = 2, List = 3 };

class Value {
public:
  using PValue = ptr<Value>;
  using ListValue = List<PValue>;
  using MapValue = Dict<Text, PValue>;
  using NullValue = struct {};

private:
  std::variant<NullValue, Text, MapValue, ListValue> m_value;

public:
  explicit Value(ValueType vt);
  explicit Value(const Text& value);
  Value();

  static PValue create_null();
  static PValue create_scalar(const Text& value = ""_t);
  static PValue create_map();
  static PValue create_list();

  [[nodiscard]] bool is_null() const;
  [[nodiscard]] bool is_scalar() const;
  [[nodiscard]] bool is_map() const;
  [[nodiscard]] bool is_list() const;

  [[nodiscard]] MapValue& as_map();
  [[nodiscard]] ListValue& as_list();
  [[nodiscard]] const MapValue& as_map() const;
  [[nodiscard]] const ListValue& as_list() const;
  [[nodiscard]] Text as_scalar() const;

public:
  [[nodiscard]] ValueType type() const;
  [[nodiscard]] TextChain to_string() const;

public:
  void set_value(const Text& txt);
  [[nodiscard]] Text get_value() const;
  [[nodiscard]] List<Text> get_array_value() const;

  void add(PValue v);
  void add(const Text& txt, PValue v);
  void add(const Text& txt);
  void add(const Text& txt, const Text& v);
  void clear();
  Value& operator[](int index) const;
  Value& operator[](const Text& key) const;
  [[nodiscard]] PValue get(int index) const;
  [[nodiscard]] PValue get(const Text& key) const;
  [[nodiscard]] bool has(const Text& key) const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] std::optional<Text> get_opt(const kl::Text& path);

public:
  void perform(const std::function<void(NullValue&)>& null_op, const std::function<void(Text&)>& scalar_op,
               const std::function<void(MapValue&)>& map_op, const std::function<void(ListValue&)>& list_op);
};

using PValue = Value::PValue;
using ListValue = Value::ListValue;
using MapValue = Value::MapValue;
using NullValue = Value::NullValue;

} // namespace kl

bool operator==(const kl::Value& v, const kl::Text& t);
