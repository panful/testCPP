set(target_name "00_04_04_MultiThreadModel")

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)
