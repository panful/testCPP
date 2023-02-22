cmake_minimum_required (VERSION 3.12)

project ("CppProject" VERSION 1.0 LANGUAGES CXX C)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)

# 设置安装目录
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/target)

# 设置编译选项
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8 /EHsc")
endif(WIN32)

add_subdirectory("source")
