#ifndef RISC_V_SIMULATOR_2024_REORDER_BUFFER_HPP
#define RISC_V_SIMULATOR_2024_REORDER_BUFFER_HPP
#include "circular_queue.hpp"
#include "instruction_unit.hpp"
#include "module.hpp"

namespace CrazyDave {
enum ROBStatus { ISSUE, WRITE,COMMIT };
#ifdef DEBUG
static const std::vector<std::string> ROBStatusString{"ISSUE", "WRITE","COMMIT"};
static auto operator<<(std::ostream& os, const ROBStatus& rhs) -> std::ostream& {
  os << ROBStatusString[rhs];
  return os;
}
#endif
struct ROBEntry {
  bool busy_{false};
  IssueType issue_type_{};
  OpCode op_{};
  addr_t addr_{};
  ROBStatus status_{};
  int dest_{};
  int val_{};
  int pred_val_{};
  int offset_{}; // only for branch
  friend auto operator<<(std::ostream& os, const ROBEntry& rhs) -> std::ostream& {
    os << rhs.busy_ << "\t" << rhs.issue_type_ << "\t" << rhs.op_ << "\t" << rhs.addr_ << "\t" << rhs.status_ << "\t" << rhs.dest_ << "\t"
       << rhs.val_ << "\t" << rhs.pred_val_;
    os << "\n";
    return os;
  }
};

class State;

class ReorderBuffer : public Module {
 private:
  void monitor(State* state);
  void commit(State* state);

 public:
  void work(State* state) override;
  void clean();
#ifdef DEBUG
  void print() {
    std::cout << "ROB:\nnum busy issue_type op addr status dest val pred_val\n";
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) {
      std::cout << "#" << i << "\t" << entries_[i];
    }
    std::cout << "\n";
  }
#endif
 private:
  CircularQueue<ROBEntry, MAX_QUEUE_SIZE> entries_;
};
}  // namespace CrazyDave

#endif  // RISC_V_SIMULATOR_2024_REORDER_BUFFER_HPP
