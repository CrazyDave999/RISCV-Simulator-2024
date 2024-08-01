#include "reservation_station.hpp"

#include "cpu.hpp"
CrazyDave::ReservationStation::ReservationStation() = default;
void CrazyDave::ReservationStation::monitor(CrazyDave::State *state) {
  // receive message came from instruction unit
  if (state->iu_rs_bus_.flag_.get()) {
    for (auto &rs_entry : entries_) {
      if (!rs_entry.busy_) {
        rs_entry = state->iu_rs_bus_.entry_.get();
        ++size_;
        break;
      }
    }
  }
  // monitor the commit message and modify the entry accordingly.
  auto &cdb = state->cdb_[2];
  if (cdb.flag_.get()) {
    for (auto &rs_entry : entries_) {
      if (!rs_entry.busy_) {
        continue;
      }
      if (rs_entry.qj_ == cdb.rob_index_.get()) {
        rs_entry.qj_ = -1;
        rs_entry.vj_ = cdb.value_.get();
      }
      if (rs_entry.qk_ == cdb.rob_index_.get()) {
        rs_entry.qk_ = -1;
        rs_entry.vk_ = cdb.value_.get();
      }
    }
  }
}
void CrazyDave::ReservationStation::clean() {
  for (auto &entry : entries_) {
    entry = RSEntry{};
  }
  size_ = 0;
}
void CrazyDave::ArithReservationStation::work(CrazyDave::State *state) {
  if (state->sys_sig_.clean_.get()) {
    clean();
    state->iu_rs_bus_.is_full_ <= (size_ >= capacity_ - 1);
    return;
  }
  // monitor the cdb, to change the entry labels
  monitor(state);

  bool flag = false;
  ALUEntry alu_entry;
  // scan the whole rs. once operands ready, send it to alu.
  for (auto &rs_entry : entries_) {
    if (rs_entry.busy_ && rs_entry.qj_ == -1 && rs_entry.qk_ == -1) {
      alu_entry.op_ = rs_entry.op_;
      alu_entry.rob_index_ = rs_entry.rob_index_;
      alu_entry.lhs_ = rs_entry.vj_;
      alu_entry.rhs_ = rs_entry.vk_;
      flag = true;
      rs_entry.busy_ = false;
      --size_;
      break;
    }
  }
  if (flag) {
    state->rs_alu_bus_.flag_ <= true;
    state->rs_alu_bus_.entry_ <= alu_entry;
  }
  state->iu_rs_bus_.is_full_ <= (size_ >= capacity_ - 1);
}
