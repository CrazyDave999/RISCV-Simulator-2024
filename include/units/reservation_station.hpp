#ifndef RISC_V_SIMULATOR_2024_RESERVATION_STATION_HPP
#define RISC_V_SIMULATOR_2024_RESERVATION_STATION_HPP

#include "circular_queue.hpp"
#include "config.hpp"
#include "module.hpp"
#include "type.hpp"

namespace CrazyDave {
struct RSEntry {
  bool busy_{false};
  OpCode op_{};
  int qj_{-1};
  int qk_{-1};
  int vj_{};
  int vk_{};
  int rob_index_{};
#ifdef DEBUG
  friend auto operator<<(std::ostream &os, const RSEntry &rhs) -> std::ostream & {
    os << rhs.busy_ << "\t" << rhs.op_ << "\t" << rhs.qj_ << "\t" << rhs.vj_ << "\t" << rhs.qk_ << "\t" << rhs.vk_ << "\t" << rhs.rob_index_;
    os << "\n";
    return os;
  }
#endif
};
class State;
class ReservationStation : public Module {
 public:
  void monitor(State *state);
  void clean();
  explicit ReservationStation();

 protected:
  std::array<RSEntry, MAX_QUEUE_SIZE> entries_;
  size_t size_{};
  size_t capacity_ = MAX_QUEUE_SIZE;
};

class ArithReservationStation : public ReservationStation {
 public:
  void work(State *state) final;
#ifdef DEBUG
  void print() {
    std::cout << "RS:\nnum busy op qj vj qk vk dest\n";
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) {
      std::cout << "#" << i << "\t" << entries_[i];
    }
    std::cout << "\n";
  }
#endif
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_RESERVATION_STATION_HPP
