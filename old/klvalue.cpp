#include "klvalue.hpp"
#include "klfs.hpp"

#include <utility>

namespace kl {
Value::Value(ValueType vt) {
  switch (vt) {
  case ValueType::Null: m_value = NullValue(); break;
  case ValueType::Scalar: m_value = Text(); break;
  case ValueType::Map: m_value = MapValue(); break;
  case ValueType::List: m_value = ListValue(); break;
  }
}
Value::Value(const Text& value) { m_value = value; }
Value::Value() { m_value = NullValue(); }

PValue Value::create_null() { return std::make_shared<Value>(); }
PValue Value::create_scalar(const Text& value) { return std::make_shared<Value>(value); }
PValue Value::create_map() { return std::make_shared<Value>(ValueType::Map); }
PValue Value::create_list() { return std::make_shared<Value>(ValueType::List); }

bool Value::is_null() const { return m_value.index() == std::to_underlying(ValueType::Null); }
bool Value::is_scalar() const { return m_value.index() == std::to_underlying(ValueType::Scalar); }
bool Value::is_map() const { return m_value.index() == std::to_underlying(ValueType::Map); }
bool Value::is_list() const { return m_value.index() == std::to_underlying(ValueType::List); }

MapValue& Value::as_map() {
  if (is_null()) {
    create_map();
  }
  return std::get<std::to_underlying(ValueType::Map)>(m_value);
}
ListValue& Value::as_list() {
  if (is_null()) {
    create_list();
  }
  return std::get<std::to_underlying(ValueType::List)>(m_value);
}
const MapValue& Value::as_map() const { return std::get<std::to_underlying(ValueType::Map)>(m_value); }
const ListValue& Value::as_list() const { return std::get<std::to_underlying(ValueType::List)>(m_value); }
Text Value::as_scalar() const { return std::get<std::to_underlying(ValueType::Scalar)>(m_value); }

ValueType Value::type() const { return static_cast<ValueType>(m_value.index()); }
void Value::set_value(const Text& txt) { m_value = txt; }
Text Value::get_value() const { return as_scalar(); }
List<Text> Value::get_array_value() const {
  if (is_scalar()) {
    return {as_scalar()};
  }
  return as_list().transform<Text>([](const PValue& p) { return p->as_scalar(); });
}

void Value::add(PValue v) { as_list().add(v); }
void Value::add(const Text& txt, PValue v) { as_map().add(txt, v); }
void Value::add(const Text& txt) { as_list().add(create_scalar(txt)); }
void Value::add(const Text& txt, const Text& v) { as_map().add(txt, create_scalar(v)); }
void Value::clear() {
  perform(
      {}, [](Text& textv) { textv.clear(); }, [](MapValue& mapv) { mapv.clear(); },
      [](ListValue& listv) { listv.clear(); });
}
Value& Value::operator[](int index) const { return *as_list()[index]; }
Value& Value::operator[](const Text& key) const { return *(as_map()[key]); }
PValue Value::get(int index) const { return as_list()[index]; }
PValue Value::get(const Text& key) const { return as_map()[key]; }
bool Value::has(const Text& key) const { return is_map() && as_map().has(key); }

size_t Value::size() const {
  if (is_null()) {
    return 0;
  }
  if (is_scalar()) {
    return 1;
  }
  if (is_list()) {
    return std::get<std::to_underlying(ValueType::List)>(m_value).size();
  }
  if (is_map()) {
    return std::get<std::to_underlying(ValueType::Map)>(m_value).size();
  }
  return 0;
}

inline void Value::perform(const std::function<void(NullValue&)>& null_op, const std::function<void(Text&)>& scalar_op,
                           const std::function<void(MapValue&)>& map_op,
                           const std::function<void(ListValue&)>& list_op) {
  if (is_null()) {
    if (null_op) {
      null_op(std::get<std::to_underlying(ValueType::Null)>(m_value));
    }
  } else if (is_scalar()) {
    if (scalar_op) {
      scalar_op(std::get<std::to_underlying(ValueType::Scalar)>(m_value));
    }
  } else if (is_map()) {
    if (map_op) {
      map_op(std::get<std::to_underlying(ValueType::Map)>(m_value));
    }
  } else if (is_list()) {
    if (list_op) {
      list_op(std::get<std::to_underlying(ValueType::List)>(m_value));
    }
  }
}

// TODO(dorin): make this one text, and the others point to fragments of it.
const Text KlValueNullText{"null"};
const Text KlValueQuoteText{R"(")"};
const Text KlValueOpenArray{"["};
const Text KlValueCloseArray{"]"};
const Text KlValueOpenCurly{"{"};
const Text KlValueCloseCurly{"}"};
const Text KlValueComma{","};
const Text KlValueColon{":"};

TextChain Value::to_string() const {
  TextChain tc;
  bool comma_needed = false;
  switch (type()) {
  case ValueType::Null: return {KlValueNullText};
  case ValueType::Scalar: return {KlValueQuoteText, as_scalar(), KlValueQuoteText};
  case ValueType::List:
    tc.add(KlValueOpenArray);
    for (const auto& v: as_list()) {
      if (comma_needed) {
        tc.add(KlValueComma);
      } else {
        comma_needed = true;
      }
      tc.add(v->to_string());
    }
    tc.add(KlValueCloseArray);
    break;
  case ValueType::Map:
    tc.add(KlValueOpenCurly);
    for (const auto& [k, v]: as_map()) {
      if (comma_needed) {
        tc.add(KlValueComma);
      } else {
        comma_needed = true;
      }
      tc.add(k);
      tc.add(KlValueColon);
      tc.add(v->to_string());
    }
    tc.add(KlValueCloseCurly);
    break;
  }
  return tc;
}

std::optional<Text> Value::get_opt(const kl::Text& path) {
  Value* v = this;
  for (const auto& val: kl::FilePath(path).breadcrumbs()) {
    if (v->is_map()) {
      auto& map = v->as_map();
      v = map.get(val).get();
    } else {
      v = nullptr;
    }
    if (v == nullptr) {
      break;
    }
  }
  if (v != nullptr && v->is_scalar()) {
    return v->get_value();
  }
  return {};
}

} // namespace kl

bool operator==(const kl::Value& v, const kl::Text& t) { return v.is_scalar() && v.get_value() == t; }
