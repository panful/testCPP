cmake_minimum_required(VERSION 3.12)

set(target_name "02_01_03_vector")

add_executable(${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION )
