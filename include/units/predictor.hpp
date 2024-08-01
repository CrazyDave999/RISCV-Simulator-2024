#ifndef RISC_V_SIMULATOR_2024_PREDICTOR_HPP
#define RISC_V_SIMULATOR_2024_PREDICTOR_HPP
#include "module.hpp"
namespace CrazyDave {
class Predictor : public Module {
 public:
  void work(State* state) override;
  [[nodiscard]] auto predict() const->bool;
  void check(bool branched);

 private:
  int state_{};
};
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_PREDICTOR_HPP
