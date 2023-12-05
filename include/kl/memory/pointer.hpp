#pragma once

namespace kl {

template <typename T>
class Pointer {
  T* m_ptr;

public:
  template <typename... Args>
  constexpr Pointer(Args&&... args) : m_ptr(new T(std::forward<Args>(args)...)) {}
  Pointer(const Pointer&) = delete;
  Pointer(Pointer&&) = delete;
  Pointer& operator=(const Pointer&) = delete;
  Pointer& operator=(Pointer&&) = delete;
  constexpr ~Pointer() { delete m_ptr; }
  constexpr T* operator->() const { return m_ptr; }
  constexpr T& operator*() const { return *m_ptr; }
};

} // namespace kl
