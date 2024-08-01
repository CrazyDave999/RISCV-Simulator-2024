#ifndef RISC_V_SIMULATOR_2024_CPU_HPP
#define RISC_V_SIMULATOR_2024_CPU_HPP
#include <iomanip>
#include <random>
#include <vector>

#include "arithmetic_logic_unit.hpp"
#include "load_store_buffer.hpp"
#include "memory_unit.hpp"
#include "predictor.hpp"
#include "register.hpp"
#include "reorder_buffer.hpp"
#include "reservation_station.hpp"
#include "type.hpp"

namespace CrazyDave {

struct State {
  int clk_{0};  // clock

  struct {
    Register<addr_t> pc_{0};
    //    Register<word_t> ir_;
    //    Wire<bool> ir_flag_;
    //    Wire<addr_t> ir_addr_;
  } iu_mem_bus;

  RegisterFile rf_;  // register file

  struct {
    Wire<bool> flag_;
    Wire<ROBEntry> entry_;
    Wire<bool> is_full_;
    Wire<int> rob_tail_;
  } iu_rob_bus_;

  struct {
    Wire<bool> flag_;
    Wire<RSEntry> entry_;
    Wire<bool> is_full_;
  } iu_rs_bus_;

  struct {
    Wire<bool> flag_;
    Wire<ALUEntry> entry_;
  } rs_alu_bus_;

  struct {
    Wire<bool> flag_;
    Wire<int> value_;
    Wire<int> rob_index_;
  } cdb_[3];  // 0 for load, 1 for alu, 2 for commit

  struct {
    Wire<bool> flag_;
    Wire<LSBEntry> entry_;
    Wire<bool> is_full_;
  } iu_lsb_bus_;

  struct {
    Wire<bool> req_flag_;
    Wire<MemoryAccessEntry> entry_;
    Wire<bool> is_busy_;

    Wire<bool> finish_flag_;
    Wire<int> value;
    Wire<int> rob_index_;
  } mem_lsb_bus_;  // memory unit notify the lsb once the load or store finish. lsb modify the entry.res_ and entry.complete accordingly.
  struct {
    Wire<int> rob_head_;
  } rob_lsb_bus_;

  struct {
    Register<bool> stall_;
    Register<bool> terminate_;
    Wire<bool> finish_;
    //    Wire<bool> abandon_;
    Wire<bool> clean_;
    Wire<addr_t> clean_pc_;
  } sys_sig_;

  Predictor *pred_;
  MemoryUnit *mem_;
#ifdef DEBUG
  ReorderBuffer *rob_;
  ArithReservationStation *rs_;
  LoadStoreBuffer *lsb_;
  InstructionUnit *iu_;
#endif
  void clean() {
    for (auto &reg : rf_) {
      reg.depend_ <= -1;
    }
  }

  void tick() {
    ++clk_;

    // Tick all registers in the register file
    for (auto &reg : rf_) {
      reg.val_.tick();
      reg.depend_.tick();
    }

    // Tick the registers in the instruction unit and memory bus
    iu_mem_bus.pc_.tick();
    //    iu_mem_bus.ir_.tick();
    //    iu_mem_bus.ir_flag_.tick();
    //    iu_mem_bus.ir_addr_.tick();

    iu_rob_bus_.flag_.tick();
    iu_rob_bus_.entry_.tick();
    iu_rob_bus_.is_full_.tick();
    iu_rob_bus_.rob_tail_.tick();

    iu_rs_bus_.flag_.tick();
    iu_rs_bus_.entry_.tick();
    iu_rs_bus_.is_full_.tick();

    rs_alu_bus_.flag_.tick();
    rs_alu_bus_.entry_.tick();

    for (auto &cdb : cdb_) {
      cdb.flag_.tick();
      cdb.value_.tick();
      cdb.rob_index_.tick();
    }

    iu_lsb_bus_.flag_.tick();
    iu_lsb_bus_.entry_.tick();
    iu_lsb_bus_.is_full_.tick();

    mem_lsb_bus_.req_flag_.tick();
    mem_lsb_bus_.entry_.tick();
    mem_lsb_bus_.is_busy_.tick();
    mem_lsb_bus_.finish_flag_.tick();
    mem_lsb_bus_.value.tick();
    mem_lsb_bus_.rob_index_.tick();

    rob_lsb_bus_.rob_head_.tick();

    sys_sig_.stall_.tick();
    sys_sig_.terminate_.tick();
    sys_sig_.finish_.tick();
    //    sys_sig_.abandon_.tick();
    sys_sig_.clean_.tick();
    sys_sig_.clean_pc_.tick();
  }
#ifdef DEBUG
  void print() {
    //    std::cout << "clk: " << clk_ << "\n";
    //
    //    std::cout << "Register file:\nnum val dep\n";
    //    for (int i = 0; i < REGISTER_NUM; ++i) {
    //      std::cout << "x" << i << "\t" << rf_[i].val_.get() << "\t" << rf_[i].depend_.get() << "\n";
    //    }
    for (int i = 0; i < REGISTER_NUM; ++i) {
      std::cout << "x" << i << "=" << std::setw(8) << std::setfill(' ') << rf_[i].val_.get() << "\n";
    }
    //    std::cout << "\n";
  }
#endif
};
class CPU {
 public:
  auto run() -> uint8_t;
  void init(addr_t addr);
  CPU();
  ~CPU();

 private:
  State state;
  std::vector<Module *> units_;
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_CPU_HPP
