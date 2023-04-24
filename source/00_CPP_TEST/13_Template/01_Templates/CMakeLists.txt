set(target_name "00_13_01_Templates")

set(CMAKE_CXX_STANDARD 17)

add_executable (${target_name} "main.cpp" "temp.h" "temp.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)