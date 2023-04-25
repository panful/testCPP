set(target_name "00_10_Const_constexpr")
set(CMAKE_CXX_STANDARD 20)
add_executable (${target_name} "main.cpp")
install(TARGETS ${target_name} RUNTIME DESTINATION .)
