cmake_minimum_required (VERSION 3.8)

set(target_name "00_15_Math")

set(CMAKE_CXX_STANDARD 20)

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)