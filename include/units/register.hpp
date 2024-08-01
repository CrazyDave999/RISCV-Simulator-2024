#ifndef RISC_V_SIMULATOR_2024_REGISTER_HPP
#define RISC_V_SIMULATOR_2024_REGISTER_HPP

#include "config.hpp"

namespace CrazyDave {

// only allow the blocking assignment
// actually there are no registers of a struct. here just for the convenience of
// cpp.
template <class T>
class Register {
 public:
  Register() = default;
  explicit Register(T t) { old_data_ = new_data_ = t; }
  [[nodiscard]] auto get() const -> T { return old_data_; }
  void set(T new_data) {
    if (updated_) {
      return;  // only allow assigning once in a cycle.
    }
    new_data_ = new_data;
    updated_ = true;
  }
  void operator<=(T new_data) { set(new_data); }
  virtual void tick() {
    old_data_ = new_data_;
    updated_ = false;
    // register will persist the value if not changed.
  }

  void force_set(T new_data) {
    new_data_ = new_data;
    updated_ = true;
  }

 protected:
  T old_data_{};
  T new_data_{};
  bool updated_{false};
};
struct RegisterFileItem {
  Register<int> val_;
  Register<int> depend_{-1};
};
using RegisterFile = RegisterFileItem[REGISTER_NUM];

template <class T>
class Wire : public Register<T> {
 public:
  void tick() {
    this->old_data_ = this->new_data_;
    this->new_data_ = T{};
    this->updated_ = false;
    // wire value will be eliminated if no new value assigned.
  }
};

}  // namespace CrazyDave

#endif  // RISC_V_SIMULATOR_2024_REGISTER_HPP
