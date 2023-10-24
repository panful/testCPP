set(target_name "00_05_custom_smart_pointer")

set(HEADERS 
    include/meta.h
    include/new.h
    include/object.h
    include/objectbase.h
    include/smartpointer.h
    include/smartpointerbase.h
)

set(SOURCES
    src/object.cpp
    src/objectbase.cpp
    src/smartpointerbase.cpp
)

add_executable (${target_name} "main.cpp" ${HEADERS} ${SOURCES})
target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${target_name} RUNTIME DESTINATION .)