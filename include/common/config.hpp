#ifndef RISC_V_SIMULATOR_2024_CONFIG_HPP
#define RISC_V_SIMULATOR_2024_CONFIG_HPP
//#define DEBUG
#include <iostream>
#include <string>
#include <vector>
namespace CrazyDave {
constexpr int MAX_QUEUE_SIZE = 32;
constexpr size_t REGISTER_NUM = 32;
constexpr size_t MEM_SIZE = 1 << 20;
enum IssueType { LOAD, STORE, ARITH_I, ARITH, BRANCH, OTHER };
#ifdef DEBUG
static const std::vector<std::string> IssueTypeString{"LOAD", "STORE", "ARITH_I", "ARITH", "BRANCH", "OTHER"};
static auto operator<<(std::ostream &os, const IssueType &rhs) -> std::ostream & {
  os << IssueTypeString[rhs];
  return os;
}
#endif
enum OpCode {
  LUI,    // Load Upper Immediate
  AUIPC,  // Add Upper Immediate to PC
  JAL,    // Jump and Link
  JALR,   // Jump and Link Register
  BEQ,    // Branch if Equal
  BNE,    // Branch if Not Equal
  BLT,    // Branch if Less Than
  BGE,    // Branch if Greater Than or Equal
  BLTU,   // Branch if Less Than Unsigned
  BGEU,   // Branch if Greater Than or Equal Unsigned
  LB,     // Load Byte
  LH,     // Load Half word
  LW,     // Load Word
  LBU,    // Load Byte Unsigned
  LHU,    // Load Half word Unsigned
  SB,     // Store Byte
  SH,     // Store Half word
  SW,     // Store Word
  ADDI,   // Add Immediate
  SLTI,   // Set Less Than Immediate
  SLTIU,  // Set Less Than Immediate Unsigned
  XORI,   // XOR Immediate
  ORI,    // OR Immediate
  ANDI,   // AND Immediate
  SLLI,   // Shift Left Logical Immediate
  SRLI,   // Shift Right Logical Immediate
  SRAI,   // Shift Right Arithmetic Immediate
  ADD,    // Add
  SUB,    // Subtract
  SLL,    // Shift Left Logical
  SLT,    // Set Less Than
  SLTU,   // Set Less Than Unsigned
  XOR,    // XOR
  SRL,    // Shift Right Logical
  SRA,    // Shift Right Arithmetic
  OR,     // OR
  AND,    // AND
};
#ifdef DEBUG
static const std::vector<std::string> OpCodeString{"LUI",   "AUIPC", "JAL", "JALR", "BEQ",  "BNE",  "BLT",  "BGE", "BLTU", "BGEU",
                                                   "LB",    "LH",    "LW",  "LBU",  "LHU",  "SB",   "SH",   "SW",  "ADDI", "SLTI",
                                                   "SLTIU", "XORI",  "ORI", "ANDI", "SLLI", "SRLI", "SRAI", "ADD", "SUB",  "SLL",
                                                   "SLT",   "SLTU",  "XOR", "SRL",  "SRA",  "OR",   "AND"};
static auto operator<<(std::ostream &os, const OpCode &rhs) -> std::ostream & {
  os << OpCodeString[rhs];
  return os;
}
#endif
}  // namespace CrazyDave
#endif  // RISC_V_SIMULATOR_2024_CONFIG_HPP
