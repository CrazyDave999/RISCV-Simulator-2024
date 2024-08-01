#include "cpu.hpp"

auto CrazyDave::CPU::run() -> uint8_t {
  while (true) {
#ifdef DEBUG
//    if (state.sys_sig_.clean_.get()) {
//      std::cout << "#####SYS SIG CLEAN#####\n";
//    }
//    if (state.sys_sig_.stall_.get()) {
//      std::cout << "#####SYS SIG STALL#####\n";
//    }
#endif
    if (state.sys_sig_.finish_.get()) {
      break;
    }
    std::shuffle(units_.begin(), units_.end(), std::mt19937(std::random_device()()));
    for (auto &unit : units_) {
      unit->work(&state);
    }
#ifdef DEBUG
//    state.print();
//    state.rob_->print();
//    state.rs_->print();
//    state.lsb_->print();
#endif
    state.tick();
  }
  return state.rf_[10].val_.get() & 255u;
}
CrazyDave::CPU::CPU() {
  auto *alu_ = new ArithmeticLogicUnit;
  auto *iu_ = new InstructionUnit;
  auto *lsb_ = new LoadStoreBuffer;
  auto *pred_ = new Predictor;
  auto *mem_ = new MemoryUnit;
  auto *rob_ = new ReorderBuffer;
  auto *rs_ = new ArithReservationStation;
#ifdef DEBUG
  state.iu_ = iu_;
  state.lsb_ = lsb_;
  state.rob_ = rob_;
  state.rs_ = rs_;
#endif
  state.pred_ = pred_;
  state.mem_ = mem_;

  units_.emplace_back(alu_);
  units_.emplace_back(iu_);
  units_.emplace_back(lsb_);
  units_.emplace_back(pred_);
  units_.emplace_back(mem_);
  units_.emplace_back(rob_);
  units_.emplace_back(rs_);
}
CrazyDave::CPU::~CPU() {
  for (auto unit : units_) {
    delete unit;
  }
}
void CrazyDave::CPU::init(addr_t addr) {}
