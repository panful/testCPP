#
# https://dandelioncloud.cn/article/details/1441580220397727746 cmake调用Python模块
#

cmake_minimum_required (VERSION 3.8)

set(target_name "06_01_CallPy")

# 如果使用的是非系统目录下的 Python 可以通过指定 Python3_ROOT_DIR 改变查找路径
# set(Python3_ROOT_DIR "C:/Program Files/Python39")
find_package(Python3 COMPONENTS Interpreter Development)

if(Python3_FOUND)
    add_executable (${target_name} "main.cpp")

    target_include_directories(${target_name} PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(${target_name} ${Python3_LIBRARIES})

    install(TARGETS ${target_name} RUNTIME DESTINATION ./)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PyFiles DESTINATION .)
else()
    message("-------------------python3 was not found-------------------")
endif()
