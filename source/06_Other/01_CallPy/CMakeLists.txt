#
# https://dandelioncloud.cn/article/details/1441580220397727746 cmake调用Python模块
#

cmake_minimum_required (VERSION 3.8)

set(target_name "06_01_CallPy")

# 如果使用的是非系统目录下的 Python 可以通过指定 Python3_ROOT_DIR 改变查找路径
# set(Python3_ROOT_DIR "C:/Program Files/Python39")
find_package(Python3 COMPONENTS Interpreter Development)

if(Python3_FOUND)
    message("-------------------python3 found-------------------")
    #message("_Python3_EXECUTABLE: ${_Python3_EXECUTABLE}")
    #message("_Python3_INCLUDE_DIR: ${_Python3_INCLUDE_DIR}")
    #message("_Python3_LIBRARY_DEBUG: ${_Python3_LIBRARY_DEBUG}")
    #message("_Python3_LIBRARY_RELEASE: ${_Python3_LIBRARY_RELEASE}")
    #message("_Python3_NumPy_INCLUDE_DIR: ${_Python3_NumPy_INCLUDE_DIR}")

    add_executable (${target_name} "main.cpp")

    target_include_directories(${target_name} PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(${target_name} ${Python3_LIBRARIES})

    install(TARGETS ${target_name} RUNTIME DESTINATION ./)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PyFiles DESTINATION .)
else()
    message("-------------------python3 was not found-------------------")
endif()
