cmake_minimum_required(VERSION 3.22)
project(RISC_V_Simulator_2024)

set(CMAKE_CXX_STANDARD 20)
include_directories(include include/units include/container include/common)
add_executable(RISC_V_Simulator_2024 main.cpp)
add_subdirectory(src)
target_link_libraries(RISC_V_Simulator_2024 src_lib)