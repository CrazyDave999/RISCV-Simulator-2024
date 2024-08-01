#include "reorder_buffer.hpp"

#include "cpu.hpp"
void CrazyDave::ReorderBuffer::work(CrazyDave::State *state) {
  if (state->sys_sig_.clean_.get()) {
    state->iu_rob_bus_.is_full_ <= (entries_.size() >= MAX_QUEUE_SIZE - 1);
    state->iu_rob_bus_.rob_tail_ <= entries_.tail_index();
    state->rob_lsb_bus_.rob_head_ <= entries_.head_index();
    return;
  }
  monitor(state);
  commit(state);
  state->iu_rob_bus_.is_full_ <= (entries_.size() >= MAX_QUEUE_SIZE - 1);
  state->iu_rob_bus_.rob_tail_ <= entries_.tail_index();
  state->rob_lsb_bus_.rob_head_ <= entries_.head_index();
}
void CrazyDave::ReorderBuffer::monitor(CrazyDave::State *state) {
  // receive message came from instruction unit
  if (state->iu_rob_bus_.flag_.get()) {
    auto rob_entry = state->iu_rob_bus_.entry_.get();
    entries_.push(rob_entry);
  }

  for (int i = 0; i < 2; ++i) {
    auto &cdb = state->cdb_[i];
    if (!cdb.flag_.get()) {
      continue;
    }
    auto index = cdb.rob_index_.get();
    entries_[index].val_ = cdb.value_.get();
    entries_[index].status_ = WRITE;
  }
}
void CrazyDave::ReorderBuffer::commit(CrazyDave::State *state) {
  static int cnt = 0;
  if (entries_.empty()) {
    if (state->sys_sig_.terminate_.get()) {
      state->sys_sig_.finish_ <= true;
    }
    return;
  }
  auto &entry = entries_.head();
  if (entry.status_ != WRITE) {
    return;
  }
  if (entry.issue_type_ == LOAD || entry.issue_type_ == ARITH || entry.issue_type_ == ARITH_I) {
    if (entry.dest_ != 0) {
      if (state->rf_[entry.dest_].depend_.get() == entries_.head_index()) {
        state->rf_[entry.dest_].depend_ <= -1;
      }
      state->rf_[entry.dest_].val_ <= entry.val_;
    }
    state->cdb_[2].flag_ <= true;
    state->cdb_[2].value_ <= entry.val_;
    state->cdb_[2].rob_index_ <= entries_.head_index();
  } else if (entry.issue_type_ == STORE) {
  } else if (entry.issue_type_ == BRANCH) {
    state->pred_->check(entry.val_);
    if (entry.val_ != entry.pred_val_) {
#ifdef DEBUG
//            std::cout << "##### predict failed #####\n";
      //      std::cout << "commit: #" << entries_.head_index() << " op_: " << entry.op_ << " addr: " << entry.addr_ << "\n\n";



      ++cnt;
      std::cout << "cnt: "<< cnt << "\n";
      std::cout << "addr: " << entry.addr_ << "\n";
      state->print();
#endif
      // predict failed. clean and correct the pc_.
      if (entry.val_) {
        state->sys_sig_.clean_pc_ <= entry.addr_ + entry.offset_;
      } else {
        state->sys_sig_.clean_pc_ <= entry.addr_ + 4;
      }
      state->sys_sig_.clean_ <= true;
      clean();

      return;
    }
  } else {
    if (entry.op_ == JALR) {
      state->sys_sig_.stall_ <= false;
      if (entry.dest_ != 0) {
        state->rf_[entry.dest_].val_ <= static_cast<int>(entry.addr_ + 4);
        state->rf_[entry.dest_].depend_ <= -1;
      }
      state->iu_mem_bus.pc_ <= entry.val_;
    }
  }

#ifdef DEBUG

  //  std::cout << "commit: #" << entries_.head_index() << " op_: " << entry.op_ << " addr: " << entry.addr_ << "\n\n";
  ++cnt;
  std::cout << "cnt: "<< cnt << "\n";
  std::cout << "addr: " << entry.addr_ << "\n";
  state->print();
#endif
  entry.status_ = COMMIT;
  entry.busy_ = false;
  entries_.pop();
}
void CrazyDave::ReorderBuffer::clean() { entries_.clean(); }
