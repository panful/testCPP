cmake_minimum_required(VERSION 3.8)

set(target_name "Framework")

set(HEADERS
    contextHub.h
    threadPool.h
)

set(SRCS
    contextHub.cpp
)

function(prepend_for_each_item src_list prepend_content)
    foreach(_item IN LISTS ${src_list})
        string(PREPEND _item ${prepend_content})
        list(APPEND _dest_list ${_item})
    endforeach()
    set(${src_list} ${_dest_list} PARENT_SCOPE)
endfunction()

prepend_for_each_item(HEADERS "include/")
prepend_for_each_item(SRCS "source/")

add_library(${target_name} SHARED ${SRCS} ${HEADERS})

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ${target_name} RUNTIME DESTINATION .)
