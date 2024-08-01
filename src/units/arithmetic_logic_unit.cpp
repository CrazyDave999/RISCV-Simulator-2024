#include "arithmetic_logic_unit.hpp"

#include "cpu.hpp"
void CrazyDave::ArithmeticLogicUnit::work(CrazyDave::State *state) {
  if (!state->rs_alu_bus_.flag_.get()) {
    return;
  }
  auto alu_entry = state->rs_alu_bus_.entry_.get();
  int val = 0;
  if (alu_entry.op_ == ADD || alu_entry.op_ == ADDI) {
    val = (alu_entry.lhs_ + alu_entry.rhs_);
  } else if (alu_entry.op_ == AND || alu_entry.op_ == ANDI) {
    val = (alu_entry.lhs_ & alu_entry.rhs_);
  } else if (alu_entry.op_ == SUB) {
    val = (alu_entry.lhs_ - alu_entry.rhs_);
  } else if (alu_entry.op_ == OR || alu_entry.op_ == ORI) {
    val = (alu_entry.lhs_ | alu_entry.rhs_);
  } else if (alu_entry.op_ == XOR || alu_entry.op_ == XORI) {
    val = (alu_entry.lhs_ ^ alu_entry.rhs_);
  } else if (alu_entry.op_ == SLL || alu_entry.op_ == SLLI) {
    val = (alu_entry.lhs_ << alu_entry.rhs_);
  } else if (alu_entry.op_ == SRA || alu_entry.op_ == SRAI) {
    val = (alu_entry.lhs_ >> alu_entry.rhs_);
  } else if (alu_entry.op_ == SRL || alu_entry.op_ == SRLI) {
    val = static_cast<int>(static_cast<uint32_t>(alu_entry.lhs_) >> static_cast<uint32_t>(alu_entry.rhs_));
  } else if (alu_entry.op_ == BEQ) {
    val = (alu_entry.lhs_ == alu_entry.rhs_);
  } else if (alu_entry.op_ == BNE) {
    val = (alu_entry.lhs_ != alu_entry.rhs_);
  } else if (alu_entry.op_ == BLT || alu_entry.op_ == SLT || alu_entry.op_ == SLTI) {
    val = (alu_entry.lhs_ < alu_entry.rhs_);
  } else if (alu_entry.op_ == BLTU || alu_entry.op_ == SLTU || alu_entry.op_ == SLTIU) {
    val = (static_cast<uint32_t>(alu_entry.lhs_) < static_cast<uint32_t>(alu_entry.rhs_));
  } else if (alu_entry.op_ == BGE) {
    val = (alu_entry.lhs_ >= alu_entry.rhs_);
  } else if (alu_entry.op_ == BGEU) {
    val = (static_cast<uint32_t>(alu_entry.lhs_) >= static_cast<uint32_t>(alu_entry.rhs_));
  } else if (alu_entry.op_ == JALR) {
    val = ((alu_entry.lhs_ + alu_entry.rhs_) & ~1);
  }
  state->cdb_[1].flag_ <= true;
  state->cdb_[1].value_ <= val;
  state->cdb_[1].rob_index_ <= alu_entry.rob_index_;
}
