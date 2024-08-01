
#ifndef RISC_V_SIMULATOR_2024_CIRCULAR_QUEUE_HPP
#define RISC_V_SIMULATOR_2024_CIRCULAR_QUEUE_HPP
#include <array>
namespace CrazyDave {
template <typename T, std::size_t L>
class CircularQueue : public std::array<T, L> {
  int head_;
  int tail_;
  int size_;

 public:
  CircularQueue() {
    head_ = tail_ = 0;
    size_ = 0;
  }
  void push(const T& x) {
    this->operator[](tail_++) = x;
    tail_ %= L;
    ++size_;
  }
  void pop() {
    head_ = (++head_) % L;
    --size_;
  }
  void clean() {
    for (auto it = this->begin(); it != this->end(); ++it) {
      *it = T{};
    }
    head_ = tail_ = 0;
    size_ = 0;
  }
  auto size() -> int { return size_; }
  auto head() -> T& { return this->operator[](head_); }
  auto empty() -> bool { return size_ == 0; }
  auto full() -> bool { return size_ == L; }
  auto head_index() -> int { return this->head_; }
  auto tail_index() -> int { return this->tail_; }
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_CIRCULAR_QUEUE_HPP
