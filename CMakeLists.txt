cmake_minimum_required (VERSION 3.12)

project ("CppProject" VERSION 1.0 LANGUAGES CXX C)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)

# 设置安装目录
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/target)

# 设置编译选项
if(MSVC)
    message("Windows MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /Zc:__cplusplus /utf-8 /EHsc")
elseif(MINGW OR UNIX)
    message("MingGW or UNIX")
    # 开启所有警告，关闭未使用的变量警告
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable")
else()
    message(FATAL_ERROR "Unrecognized compiler")
endif()

add_subdirectory("source")
