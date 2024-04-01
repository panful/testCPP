cmake_minimum_required (VERSION 3.8)

set(target_name "05_01_02_SocketClient")

# 将源代码添加到此项目的可执行文件。
add_executable (${target_name} "main.cpp")

target_link_libraries(${target_name} wsock32 ws2_32)

# 将库文件，可执行文件，头文件安装到指定目录
install(TARGETS ${target_name} RUNTIME DESTINATION ./)