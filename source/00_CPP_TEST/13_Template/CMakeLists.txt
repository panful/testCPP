cmake_minimum_required (VERSION 3.8)

set(target_name "00_13_Template")

set(CMAKE_CXX_STANDARD 17)

add_executable (${target_name} "main.cpp" "temp.h" "temp.cpp" "main2.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)