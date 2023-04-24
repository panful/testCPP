cmake_minimum_required (VERSION 3.8)

set(target_name "02_004_Iterator")

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)