cmake_minimum_required (VERSION 3.8)

project ("CppProject")

set(CMAKE_CXX_STANDARD 20)
if(WIN32)
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8")
endif(WIN32)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/target)

add_subdirectory("source")

