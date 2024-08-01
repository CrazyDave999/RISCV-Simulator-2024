#include "load_store_buffer.hpp"

#include "cpu.hpp"
void CrazyDave::LoadStoreBuffer::monitor(CrazyDave::State *state) {
  if (state->iu_lsb_bus_.flag_.get()) {
    auto entry = state->iu_lsb_bus_.entry_.get();
    entries_.push(entry);
  }

  auto cdb = state->cdb_[2];
  if (cdb.flag_.get()) {
    for (auto &entry : entries_) {
      if (!entry.busy) {
        continue;
      }
      if (entry.qj_ == cdb.rob_index_.get()) {
        entry.qj_ = -1;
        entry.vj_ = cdb.value_.get();
      }
      if (entry.qk_ == cdb.rob_index_.get()) {
        entry.qk_ = -1;
        entry.vk_ = cdb.value_.get();
      }
    }
  }

  // receive message for load finish came from memory
  if (state->mem_lsb_bus_.finish_flag_.get()) {
    for (auto &entry : entries_) {
      if (!entry.busy) {
        continue;
      }
      if (entry.rob_index_ == state->mem_lsb_bus_.rob_index_.get()) {
        entry.res_ = state->mem_lsb_bus_.value.get();
        entry.complete_ = true;
        break;
      }
    }
  }
}

void CrazyDave::LoadStoreBuffer::work(CrazyDave::State *state) {
  if (state->sys_sig_.clean_.get()) {
    clean();
    state->iu_lsb_bus_.is_full_ <= (entries_.size() >= MAX_QUEUE_SIZE - 1);
    return;
  }

  monitor(state);

  // if memory unit is not busy, find an instruction that operands have been ready. send it to memory.
  if (!state->mem_lsb_bus_.is_busy_.get()) {
    bool flag = false;
    MemoryAccessEntry mem_entry;
    auto rob_head_ = state->rob_lsb_bus_.rob_head_.get();

    for (auto &entry : entries_) {
      if (entry.issue_type_ == STORE && rob_head_ != entry.rob_index_) {
        continue;
      }
      if (entry.busy && !entry.complete_ && entry.qj_ == -1 && entry.qk_ == -1) {
        flag = true;
        addr_t addr = entry.vj_ + sign_extend(entry.imm_, 12);
        entry.addr_ = static_cast<int>(addr);
        int val = entry.vk_;
        mem_entry = {entry.op_, entry.issue_type_, entry.rob_index_, val, addr};
        break;
      }
    }
    if (flag) {
      state->mem_lsb_bus_.req_flag_ <= true;
      state->mem_lsb_bus_.entry_ <= mem_entry;
    }
  }

  // check if the head is complete. if so, commit it.
  auto &entry = entries_.head();
  if (entry.complete_) {
    state->cdb_[0].flag_ <= true;
    state->cdb_[0].rob_index_ <= entry.rob_index_;
    if (entry.issue_type_ == LOAD) {
      state->cdb_[0].value_ <= entry.res_;
    } else {
      // for a store instruction.
      // check if there is some following load instructions have been complete.
      // if there are some, mark them incomplete.
      for (auto &ld_entry : entries_) {
        if (ld_entry.busy && ld_entry.issue_type_ == LOAD && ld_entry.complete_) {
          bool conflict = (entry.addr_ <= ld_entry.addr_ + ld_entry.length_ - 1 && entry.addr_ + entry.length_ - 1 >= ld_entry.addr_);
          if (conflict) {
            ld_entry.complete_ = false;
          }
        }
      }
    }
    entry.busy = false;
    entry.complete_ = false;
    entries_.pop();
  }
  state->iu_lsb_bus_.is_full_ <= (entries_.size() >= MAX_QUEUE_SIZE - 1);
}
void CrazyDave::LoadStoreBuffer::clean() { entries_.clean(); }
