#include "utils.hpp"
namespace CrazyDave {
auto sign_extend(word_t val, int width) -> int {
  bool neg = val & (1 << (width - 1));
  if (neg) {
    uint32_t mask = ~((1 << width) - 1);
    return static_cast<int>(val | mask);
  }
  return static_cast<int>(val);
}
}