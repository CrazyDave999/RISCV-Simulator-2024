#include "memory_unit.hpp"

#include "cpu.hpp"

void CrazyDave::MemoryUnit::work(CrazyDave::State *state) {
  if (state->sys_sig_.clean_.get()) {
    counter_ = 0;
    busy_ = false;
    state->mem_lsb_bus_.is_busy_ <= false;
    cur_entry_ = MemoryAccessEntry();
    return;
  }
  if (counter_ > 0) {
    --counter_;
    state->mem_lsb_bus_.is_busy_ <= true;
    return;
  }
  if (busy_) {
    // run the last instruction
    auto addr = cur_entry_.addr_;

    state->mem_lsb_bus_.finish_flag_ <= true;
    state->mem_lsb_bus_.rob_index_ <= cur_entry_.rob_index_;
    if (cur_entry_.issue_type_ == LOAD) {
      int val = 0;
      switch (cur_entry_.op_) {
        case LB:
        case LBU:
          val = mem_[addr];
          break;
        case LH:
        case LHU:
          val = mem_[addr] + (static_cast<int>(mem_[addr + 1]) << 8);
          break;
        case LW:
          val = mem_[addr] + (static_cast<int>(mem_[addr + 1]) << 8) + (static_cast<int>(mem_[addr + 2]) << 16) +
                (static_cast<int>(mem_[addr + 3]) << 24);
          break;
        default:
          break;
      }
      if (cur_entry_.op_ == LB) {
        val = sign_extend(val, 8);
      } else if (cur_entry_.op_ == LH) {
        val = sign_extend(val, 16);
      }
      state->mem_lsb_bus_.value <= val;
    } else {
      auto val = cur_entry_.val_;
      if (cur_entry_.op_ == SB || cur_entry_.op_ == SH || cur_entry_.op_ == SW) {
        mem_[addr] = val;
      }
      if (cur_entry_.op_ == SH || cur_entry_.op_ == SW) {
        mem_[addr + 1] = val >> 8;
      }
      if (cur_entry_.op_ == SW) {
        mem_[addr + 2] = val >> 16;
        mem_[addr + 3] = val >> 24;
      }
    }
    busy_ = false;
  }
  if (state->mem_lsb_bus_.req_flag_.get()) {
    cur_entry_ = state->mem_lsb_bus_.entry_.get();
    counter_ += 3;
    busy_ = true;
  }
  state->mem_lsb_bus_.is_busy_ <= busy_;
}
auto CrazyDave::MemoryUnit::fetch_word(addr_t addr) -> word_t {
  word_t ins_bin = 0;
  for (int i = 0; i < sizeof(word_t) / sizeof(byte_t); ++i) {
    ins_bin |= mem_[addr + i] << (i * 8 * sizeof(byte_t));
  }
  return ins_bin;
}
CrazyDave::Memory::Memory() {
  std::string line;
  addr_t addr = 0;
  while (std::getline(std::cin, line)) {
    line.erase(line.find_last_not_of(' ') + 1);
    if (line[0] == '@') {
      std::istringstream addr_stream(line.substr(1));
      addr_stream >> std::hex >> addr;
    } else {
      std::istringstream data_stream(line);
      std::string byte_str;

      while (data_stream >> byte_str) {
        auto val = static_cast<byte_t>(std::strtol(byte_str.c_str(), nullptr, 16));
        data_[addr++] = val;
      }
    }
  }
}
