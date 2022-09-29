cmake_minimum_required (VERSION 3.8)

set(target_name "00_16_Static_namespace")

add_executable (${target_name} "main.cpp" "header1.h" "header2.h" "header3.h")

install(TARGETS ${target_name} RUNTIME DESTINATION ./)