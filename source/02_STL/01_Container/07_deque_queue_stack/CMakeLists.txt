cmake_minimum_required (VERSION 3.8)

set(target_name "02_01_07_stack_queue_deque")

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)