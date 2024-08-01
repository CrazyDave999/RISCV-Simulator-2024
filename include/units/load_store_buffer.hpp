#ifndef RISC_V_SIMULATOR_2024_LOAD_STORE_BUFFER_HPP
#define RISC_V_SIMULATOR_2024_LOAD_STORE_BUFFER_HPP
#include "circular_queue.hpp"
#include "config.hpp"
#include "module.hpp"
#include "type.hpp"
namespace CrazyDave {
struct LSBEntry {
  bool busy{};
  OpCode op_{};
  IssueType issue_type_{};
  int qj_{};
  int qk_{};
  int vj_{};
  int vk_{};
  int imm_{};
  int rob_index_{};
  addr_t addr_{};
  int length_{};
  bool complete_{};
  int res_{};
#ifdef DEBUG
  friend auto operator<<(std::ostream& os, const LSBEntry& rhs) -> std::ostream& {
    os << rhs.busy << "\t" << rhs.op_ << "\t" << rhs.issue_type_ << "\t" << rhs.qj_ << "\t" << rhs.vj_ << "\t" << rhs.qk_ << "\t" << rhs.vk_
       << "\t" << rhs.imm_ << "\t" << rhs.rob_index_ << "\t" << rhs.addr_ << "\t" << rhs.length_ << "\t" << rhs.complete_ << "\t"
       << rhs.res_;
    os << "\n";
    return os;
  }
#endif
};
class LoadStoreBuffer : public Module {
 private:
  void monitor(State* state);

 public:
  void work(State* state) override;
  void clean();
#ifdef DEBUG
  void print() {
    std::cout << "LSB:\nnum\tbusy\top\tissue_type\tqj\tvj\tqk\tvk\timm\trob_index\taddr\tlength\tcomplete\tres\n";
    for (int i = 0; i < MAX_QUEUE_SIZE; ++i) {
      std::cout << "#" << i << "\t" << entries_[i];
    }
    std::cout << "\n";
  }
#endif

 private:
  CircularQueue<LSBEntry, 32> entries_;
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_LOAD_STORE_BUFFER_HPP
