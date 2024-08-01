#include "predictor.hpp"

#include "cpu.hpp"
// Let's first implement a 2-bit saturating predictor
void CrazyDave::Predictor::work(CrazyDave::State *state) {}

auto CrazyDave::Predictor::predict() const -> bool { return state_ > 1; }
void CrazyDave::Predictor::check(bool branched) {
  if (branched) {
    state_ = std::min(3, state_ + 1);
  } else {
    state_ = std::max(0, state_ - 1);
  }
}
