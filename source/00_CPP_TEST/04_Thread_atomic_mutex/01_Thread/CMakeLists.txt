set(target_name "00_04_01_Thread")

add_executable (${target_name} "main.cpp" "myUtility.hpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)
