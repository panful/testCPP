set(target_name "01_09_cxx20")

set(CMAKE_CXX_STANDARD 20)

add_executable(${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)