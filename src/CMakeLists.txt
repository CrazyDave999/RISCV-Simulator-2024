set(PROJECT_NAME ${CMAKE_PROJECT_NAME})
set(
        units_src
        units/reorder_buffer.cpp
        units/reservation_station.cpp
        units/arithmetic_logic_unit.cpp
        units/instruction_unit.cpp
        units/memory_unit.cpp
        units/load_store_buffer.cpp
        units/predictor.cpp
)
set(
        common_src
        common/utils.cpp
)

add_library(src_lib cpu.cpp ${units_src} ${common_src})