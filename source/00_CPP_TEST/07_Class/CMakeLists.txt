cmake_minimum_required (VERSION 3.8)

set(target_name "00_07_Class")

add_executable (${target_name} "main.cpp" "header.h")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)