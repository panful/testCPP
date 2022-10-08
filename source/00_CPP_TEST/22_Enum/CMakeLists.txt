cmake_minimum_required (VERSION 3.8)

set(target_name "00_22_Enum")

add_executable (${target_name} "main.cpp" "magic_enum.hpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)
