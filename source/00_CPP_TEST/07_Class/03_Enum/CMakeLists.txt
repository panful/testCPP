set(target_name "00_07_03_Enum")

add_executable (${target_name} "main.cpp" "magic_enum.hpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)
