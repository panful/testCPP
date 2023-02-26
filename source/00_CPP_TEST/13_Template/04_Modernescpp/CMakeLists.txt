set(target_name "00_13_04_Modernescpp")

set(CMAKE_CXX_STANDARD 17)

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)