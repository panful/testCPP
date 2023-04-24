cmake_minimum_required (VERSION 3.8)

set(target_name "05_01_03_02_Client")

add_executable(${target_name} "main.cpp" "socket.h" "socket.cpp")

target_link_libraries(${target_name} PRIVATE wsock32 ws2_32)

install(TARGETS ${target_name} RUNTIME DESTINATION ./)
