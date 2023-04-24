cmake_minimum_required(VERSION 3.8)

set(target_name "04_01_Factory")

add_executable(${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)