#ifndef RISC_V_SIMULATOR_2024_MODULE_HPP
#define RISC_V_SIMULATOR_2024_MODULE_HPP

namespace CrazyDave {
class State;
class Module {
 public:
  virtual void work(State *state) = 0;
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_MODULE_HPP
