set(target_name "00_04_06_LinuxThread")

if(NOT UNIX)
    return()
endif()

add_executable (${target_name} "main.cpp")

install(TARGETS ${target_name} RUNTIME DESTINATION .)
