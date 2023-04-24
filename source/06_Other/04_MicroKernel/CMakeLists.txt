cmake_minimum_required(VERSION 3.8)

set(target_name "06_04_MicroKernel")

add_subdirectory("Framework")

add_executable(${target_name} "main.cpp")
target_link_libraries(${target_name} PRIVATE Framework)
target_include_directories(${target_name} PRIVATE Framework/include)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
