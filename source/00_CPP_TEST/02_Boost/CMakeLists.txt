cmake_minimum_required (VERSION 3.8)

set(target_name "00_02_boost_test")

#指定boost根目录
if(WIN32)
    set(BOOST_ROOT "C:/Boost")
endif()

#设置不适用系统的的boost的库
set(BOOST_NO_SYSTEM_PATHS ON)

#设置使用静态的boost库
set(Boost_USE_STATIC_LIBS ON)

#查找boost库
find_package(Boost COMPONENTS thread filesystem locale system)

if(BOOST_FOUND)

    #MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
    #MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
    #MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
    #MESSAGE( STATUS "Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}.")

    add_executable (${target_name} "main.cpp")

    # 不知道为啥boost不能用target_include_directories target_link_directories
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries (${target_name} ${Boost_LIBRARIES})

    install(TARGETS ${target_name} RUNTIME DESTINATION ./)
else()
    message("===== boost was not found =====")
endif()
