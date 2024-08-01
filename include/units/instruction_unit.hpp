#ifndef RISC_V_SIMULATOR_2024_INSTRUCTION_UNIT_HPP
#define RISC_V_SIMULATOR_2024_INSTRUCTION_UNIT_HPP
#include <bit>

#include "circular_queue.hpp"
#include "config.hpp"
#include "module.hpp"
#include "type.hpp"
#include "utils.hpp"

namespace CrazyDave {
struct Instruction {
  addr_t addr_{};
  OpCode op_{};
  IssueType issue_type_{};
  int rs1_{};
  int rs2_{};
  int rd_{};
  int imm_{};
  enum { R, I, S, B, U, J } type_{};
  explicit Instruction(word_t ins_bin);
};
struct UnknownType {
  unsigned int opcode : 7;
  unsigned int unknown : 25;
};
struct RType {
  unsigned int opcode : 7;
  unsigned int rd : 5;
  unsigned int funct3 : 3;
  unsigned int rs1 : 5;
  unsigned int rs2 : 5;
  unsigned int funct7 : 7;
};

struct IType {
  unsigned int opcode : 7;
  unsigned int rd : 5;
  unsigned int funct3 : 3;
  unsigned int rs1 : 5;
  unsigned int imm_11_0 : 12;
  [[nodiscard]] auto get_funct7() const -> unsigned int { return imm_11_0 >> 5; }

  [[nodiscard]] auto get_shamt() const -> unsigned int { return imm_11_0 & 0b11111; }
};

struct SType {
  unsigned int opcode : 7;
  unsigned int imm_4_0 : 5;
  unsigned int funct3 : 3;
  unsigned int rs1 : 5;
  unsigned int rs2 : 5;
  unsigned int imm_11_5 : 7;
};

struct BType {
  unsigned int opcode : 7;
  unsigned int imm_11 : 1;
  unsigned int imm_4_1 : 4;
  unsigned int funct3 : 3;
  unsigned int rs1 : 5;
  unsigned int rs2 : 5;
  unsigned int imm_10_5 : 6;
  unsigned int imm_12 : 1;
  [[nodiscard]] auto get_imm() const -> int { return imm_12 << 12 | imm_11 << 11 | imm_10_5 << 5 | imm_4_1 << 1; }
};

struct UType {
  unsigned int opcode : 7;
  unsigned int rd : 5;
  unsigned int imm_31_12 : 20;
  [[nodiscard]] auto get_imm() const -> int { return imm_31_12 << 12; }
};
struct JType {
  unsigned int opcode : 7;
  unsigned int rd : 5;
  unsigned int imm_19_12 : 8;
  unsigned int imm_11 : 1;
  unsigned int imm_10_1 : 10;
  unsigned int imm_20 : 1;
  [[nodiscard]] auto get_imm() const -> int { return imm_20 << 20 | imm_19_12 << 12 | imm_11 << 11 | imm_10_1 << 1; }
};
struct InstructionEntry{
  word_t ins_bin_{};
  addr_t addr_{};
};

class InstructionUnit : public Module {
 private:
  void monitor(State* state);

 public:
  void work(State* state) override;
  void clean();

 private:
  CircularQueue<InstructionEntry, 32> ins_queue_;
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_INSTRUCTION_UNIT_HPP
