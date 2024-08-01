#ifndef RISC_V_SIMULATOR_2024_MEMORY_UNIT_HPP
#define RISC_V_SIMULATOR_2024_MEMORY_UNIT_HPP
#include "config.hpp"
#include "instruction_unit.hpp"
#include "module.hpp"
#include "type.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
namespace CrazyDave {
struct Memory {
  byte_t data_[MEM_SIZE]{};
  auto operator[](addr_t addr) -> byte_t& { return data_[addr]; }
  Memory();
};

struct MemoryAccessEntry {
  OpCode op_{};
  IssueType issue_type_{};
  int rob_index_{};
  int val_{};
  addr_t addr_{};
};

class MemoryUnit : public Module {
 public:
  void work(State* state) override;
  auto fetch_word(addr_t addr) -> word_t;

 private:
  Memory mem_{};
  MemoryAccessEntry cur_entry_;
  int counter_{0};
  bool busy_{false};
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_MEMORY_UNIT_HPP
