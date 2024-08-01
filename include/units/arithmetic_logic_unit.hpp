#ifndef RISC_V_SIMULATOR_2024_ARITHMETIC_LOGIC_UNIT_HPP
#define RISC_V_SIMULATOR_2024_ARITHMETIC_LOGIC_UNIT_HPP

#include "module.hpp"
#include "config.hpp"
#include "circular_queue.hpp"

namespace CrazyDave {
struct ALUEntry {
  OpCode op_{};
  int rob_index_{};
  int lhs_{}, rhs_{};
};
class ArithmeticLogicUnit : public Module {
 public:
  void work(State* state) override;

 private:
  CircularQueue<ALUEntry, 100> alu_queue_;
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_ARITHMETIC_LOGIC_UNIT_HPP
