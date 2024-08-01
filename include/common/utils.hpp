#ifndef RISC_V_SIMULATOR_2024_UTILS_HPP
#define RISC_V_SIMULATOR_2024_UTILS_HPP
#include "type.hpp"
namespace CrazyDave {
auto sign_extend(word_t val, int width) -> int;
// auto zero_extent(word_t val) -> int { return static_cast<int>(val); }

}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_UTILS_HPP
