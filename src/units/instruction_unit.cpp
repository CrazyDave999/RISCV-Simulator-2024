#include "instruction_unit.hpp"

#include "cpu.hpp"

void CrazyDave::InstructionUnit::work(CrazyDave::State *state) {
  if (state->sys_sig_.clean_.get()) {
    state->sys_sig_.stall_ <= false;
    clean();
    state->clean();
    state->iu_mem_bus.pc_ <= state->sys_sig_.clean_pc_.get();
    ins_queue_.clean();
    return;
  }

  monitor(state);  // fetch
  if (ins_queue_.empty() || state->sys_sig_.terminate_.get()) {
    return;
  }
  if (state->iu_lsb_bus_.is_full_.get() || state->iu_rob_bus_.is_full_.get() || state->iu_rs_bus_.is_full_.get()) {
    ins_queue_.pop();
    return;
  }
  auto ins_entry = ins_queue_.head();
  auto ins_bin = ins_entry.ins_bin_;

  Instruction ins{ins_bin};  // decode
  ins.addr_ = ins_entry.addr_;

  if (ins_bin == 0x0ff00513) {
    state->sys_sig_.terminate_ <= true;
    return;
  }
  // issue
  int pred_val = 0;
  bool modified = false;
  if (ins.op_ == JAL) {
    modified = true;
    state->iu_mem_bus.pc_ <= ins.addr_ + sign_extend(ins.imm_, 20);
    ins.op_ = ADDI;
    ins.issue_type_ = ARITH_I;
    ins.rs1_ = 0;
    ins.imm_ = static_cast<int>(ins.addr_ + 4);

  } else if (ins.op_ == LUI) {
    modified = true;
    state->iu_mem_bus.pc_ <= state->iu_mem_bus.pc_.get() + 4;
    ins.op_ = ADDI;
    ins.issue_type_ = ARITH_I;
    ins.rs1_ = 0;
  } else if (ins.op_ == AUIPC) {
    modified = true;
    state->iu_mem_bus.pc_ <= state->iu_mem_bus.pc_.get() + 4;
    ins.op_ = ADDI;
    ins.issue_type_ = ARITH_I;
    ins.rs1_ = 0;
    ins.imm_ = static_cast<int>(state->iu_mem_bus.pc_.get()) + ins.imm_;
  }
  auto rob_tail = state->iu_rob_bus_.rob_tail_.get();
  if (state->iu_rob_bus_.flag_.get()) {
    // in this cycle, rob should be pushed. so the tail should increase.
    rob_tail = (rob_tail + 1) % MAX_QUEUE_SIZE;
  }

  int offset = 0;  // only for branch
  if (ins.issue_type_ == LOAD || ins.issue_type_ == STORE) {
    state->iu_mem_bus.pc_ <= state->iu_mem_bus.pc_.get() + 4;
    // for a load-store instruction, create an ROB entry and an LSB entry.

    int length = 0;
    switch (ins.op_) {
      case LB:
      case LBU:
      case SB:
        length = 1;
        break;
      case LH:
      case LHU:
      case SH:
        length = 2;
        break;
      case LW:
      case SW:
        length = 4;
        break;
      default:
        break;
    }
    LSBEntry lsb_entry{true, ins.op_, ins.issue_type_, -1, -1, -1, -1, ins.imm_, rob_tail, 0, length, false, 0};
    auto depend1 = state->rf_[ins.rs1_].depend_.get();
    if (depend1 == -1) {
      lsb_entry.vj_ = state->rf_[ins.rs1_].val_.get();
    } else {
      lsb_entry.qj_ = depend1;
    }
    if (ins.issue_type_ == STORE) {
      auto depend2 = state->rf_[ins.rs2_].depend_.get();
      if (depend2 == -1) {
        lsb_entry.vk_ = state->rf_[ins.rs2_].val_.get();
      } else {
        lsb_entry.qk_ = depend2;
      }
    }

    state->iu_lsb_bus_.flag_ <= true;
    state->iu_lsb_bus_.entry_ <= lsb_entry;
  } else if (ins.issue_type_ == ARITH || ins.issue_type_ == ARITH_I || ins.op_ == JALR) {
    if (!modified) {
      state->iu_mem_bus.pc_ <= state->iu_mem_bus.pc_.get() + 4;
    }
    // for a non load-store instruction, create an ROB entry and an RS entry.

    if (ins.op_ == JALR) {
      state->sys_sig_.stall_ <= true;
    }
    RSEntry rs_entry{true, ins.op_, -1, -1, -1, -1, rob_tail};
    auto depend1 = state->rf_[ins.rs1_].depend_.get();
    if (depend1 == -1) {
      rs_entry.vj_ = state->rf_[ins.rs1_].val_.get();
    } else {
      rs_entry.qj_ = depend1;
    }
    if (ins.issue_type_ == ARITH) {
      auto depend2 = state->rf_[ins.rs2_].depend_.get();
      if (depend2 == -1) {
        rs_entry.vk_ = state->rf_[ins.rs2_].val_.get();
      } else {
        rs_entry.qk_ = depend2;
      }
    } else {
      rs_entry.vk_ = sign_extend(ins.imm_, 12);
    }
    state->iu_rs_bus_.flag_ <= true;
    state->iu_rs_bus_.entry_ <= rs_entry;
  } else if (ins.issue_type_ == BRANCH) {
    if (state->pred_->predict()) {
      pred_val = 1;

      state->iu_mem_bus.pc_ <= ins.addr_ + sign_extend(ins.imm_, 12);
    } else {
      pred_val = 0;
      state->iu_mem_bus.pc_ <= state->iu_mem_bus.pc_.get() + 4;
    }
    offset = sign_extend(ins.imm_, 12);

    RSEntry rs_entry{true, ins.op_, -1, -1, -1, -1, rob_tail};
    auto depend1 = state->rf_[ins.rs1_].depend_.get();
    if (depend1 == -1) {
      rs_entry.vj_ = state->rf_[ins.rs1_].val_.get();
    } else {
      rs_entry.qj_ = depend1;
    }

    auto depend2 = state->rf_[ins.rs2_].depend_.get();
    if (depend2 == -1) {
      rs_entry.vk_ = state->rf_[ins.rs2_].val_.get();
    } else {
      rs_entry.qk_ = depend2;
    }

    state->iu_rs_bus_.flag_ <= true;
    state->iu_rs_bus_.entry_ <= rs_entry;
  }
  if (ins.issue_type_ != BRANCH && ins.rd_ != -1 && ins.rd_ != 0) {
    // modify the dependency to itself
    state->rf_[ins.rd_].depend_.force_set(rob_tail);
  }
  ROBEntry rob_entry{true, ins.issue_type_, ins.op_, ins.addr_, ISSUE, ins.rd_, -1, pred_val, offset};
  state->iu_rob_bus_.flag_ <= true;
  state->iu_rob_bus_.entry_ <= rob_entry;
  if (ins.op_ == JALR) {
    ins_queue_.clean();
  } else {
    ins_queue_.pop();
  }
}
void CrazyDave::InstructionUnit::monitor(CrazyDave::State *state) {
  if (state->sys_sig_.stall_.get()) {
    return;
  }

  // receive instruction from memory
  auto ins_bin = state->mem_->fetch_word(state->iu_mem_bus.pc_.get());
  auto ins_addr = state->iu_mem_bus.pc_.get();
  InstructionEntry entry{ins_bin, ins_addr};
  ins_queue_.push(entry);
}
void CrazyDave::InstructionUnit::clean() { ins_queue_.clean(); }

CrazyDave::Instruction::Instruction(word_t ins_bin) {
  auto unknown_ins = std::bit_cast<UnknownType>(ins_bin);
  issue_type_ = OTHER;
  switch (unknown_ins.opcode) {
    case 0b0110111:
      type_ = U, op_ = LUI;
      break;
    case 0b0010111:
      type_ = U, op_ = AUIPC;
      break;
    case 0b1101111:
      type_ = J, op_ = JAL;
      break;
    case 0b1100111:
      type_ = I, op_ = JALR;
      break;
    case 0b1100011:
      type_ = B;
      break;
    case 0b0000011:
    case 0b0010011:
      type_ = I;
      break;
    case 0b0100011:
      type_ = S;
      break;
    case 0b0110011:
      type_ = R;
      break;
    default:
      break;
  }
  if (type_ == R) {
    auto r_ins = std::bit_cast<RType>(ins_bin);
    rd_ = r_ins.rd;
    rs1_ = r_ins.rs1;
    rs2_ = r_ins.rs2;
    issue_type_ = ARITH;
    switch (r_ins.funct3) {
      case 0b000:
        switch (r_ins.funct7) {
          case 0b0000000:
            op_ = ADD;
            break;
          case 0b0100000:
            op_ = SUB;
            break;
          default:
            break;
        }
        break;
      case 0b001:
        op_ = SLL;
        break;
      case 0b010:
        op_ = SLT;
        break;
      case 0b011:
        op_ = SLTU;
        break;
      case 0b100:
        op_ = XOR;
        break;
      case 0b101:
        switch (r_ins.funct7) {
          case 0b0000000:
            op_ = SRL;
            break;
          case 0b0100000:
            op_ = SRA;
            break;
          default:
            break;
        }
        break;
      case 0b110:
        op_ = OR;
        break;
      case 0b111:
        op_ = AND;
        break;
    }
  } else if (type_ == I) {
    auto i_ins = std::bit_cast<IType>(ins_bin);
    rd_ = i_ins.rd;
    rs1_ = i_ins.rs1;
    imm_ = i_ins.imm_11_0;
    switch (i_ins.opcode) {
      case 0b0000011:
        issue_type_ = LOAD;
        switch (i_ins.funct3) {
          case 0b000:
            op_ = LB;
            break;
          case 0b001:
            op_ = LH;
            break;
          case 0b010:
            op_ = LW;
            break;
          case 0b100:
            op_ = LBU;
            break;
          case 0b101:
            op_ = LHU;
            break;
          default:
            break;
        }
        break;
      case 0b0010011:
        issue_type_ = ARITH_I;
        switch (i_ins.funct3) {
          case 0b000:
            op_ = ADDI;
            break;
          case 0b010:
            op_ = SLTI;
            break;
          case 0b011:
            op_ = SLTIU;
            break;
          case 0b100:
            op_ = XORI;
            break;
          case 0b110:
            op_ = ORI;
            break;
          case 0b111:
            op_ = ANDI;
            break;
          case 0b001:
            imm_ = static_cast<int>(i_ins.get_shamt());
            op_ = SLLI;
            break;
          case 0b101:
            switch (i_ins.get_funct7()) {
              case 0b0000000:
                op_ = SRLI;
                break;
              case 0b0100000:
                op_ = SRAI;
                break;
              default:
                break;
            }
            break;
        }
        break;
      default:
        break;
    }
  } else if (type_ == S) {
    auto s_ins = std::bit_cast<SType>(ins_bin);
    rs1_ = s_ins.rs1;
    rs2_ = s_ins.rs2;
    imm_ = s_ins.imm_11_5 << 5 | s_ins.imm_4_0;
    issue_type_ = STORE;
    switch (s_ins.funct3) {
      case 0b000:
        op_ = SB;
        break;
      case 0b001:
        op_ = SH;
        break;
      case 0b010:
        op_ = SW;
        break;
      default:
        break;
    }
  } else if (type_ == B) {
    auto b_ins = std::bit_cast<BType>(ins_bin);
    rs1_ = b_ins.rs1;
    rs2_ = b_ins.rs2;
    imm_ = b_ins.get_imm();
    issue_type_ = BRANCH;
    switch (b_ins.funct3) {
      case 0b000:
        op_ = BEQ;
        break;
      case 0b001:
        op_ = BNE;
        break;
      case 0b100:
        op_ = BLT;
        break;
      case 0b101:
        op_ = BGE;
        break;
      case 0b110:
        op_ = BLTU;
        break;
      case 0b111:
        op_ = BGEU;
        break;
      default:
        break;
    }
  } else if (type_ == U) {
    auto u_ins = std::bit_cast<UType>(ins_bin);
    rd_ = u_ins.rd;
    imm_ = u_ins.get_imm();
  } else {
    auto j_ins = std::bit_cast<JType>(ins_bin);
    rd_ = j_ins.rd;
    imm_ = j_ins.get_imm();
  }
}
