cmake_minimum_required (VERSION 3.12)

project ("CppProject" VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置编译选项
if(MSVC)
    message("Windows MSVC")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /Zc:__cplusplus /utf-8 /EHsc")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/target)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/target)
elseif(MINGW OR UNIX)
    message("MingGW or UNIX")

    # 开启所有警告，关闭未使用的变量警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/target)
else()
    message(FATAL_ERROR "Unrecognized compiler")
endif()

add_subdirectory("source")
