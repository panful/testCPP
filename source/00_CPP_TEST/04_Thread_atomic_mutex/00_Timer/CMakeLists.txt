set(target_name "00_04_00_Timer")

add_executable (${target_name} "main.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/../00_Timer/myUtility.hpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)
