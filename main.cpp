#include <cstdio>
#include <iostream>

#include "cpu.hpp"
int main() {

#ifdef DEBUG
  freopen("../testcases/queens.data", "r", stdin);
  freopen("../log", "w", stdout);
#else
//  freopen("../testcases/queens.data", "r", stdin);
#endif
  CrazyDave::CPU RISCV32I_cpu;
  RISCV32I_cpu.init(0);
  auto ret = RISCV32I_cpu.run();
  printf("%u\n", ret);
  return 0;
}
