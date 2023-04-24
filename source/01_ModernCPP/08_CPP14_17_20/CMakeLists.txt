cmake_minimum_required (VERSION 3.8)

set(target_name "01_08_CPP14_17_20")

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)
