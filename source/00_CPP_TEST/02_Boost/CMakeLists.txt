set(target_name "00_02_boost_test")

#指定boost根目录
#if(WIN32)
#    set(BOOST_ROOT "C:/Boost")
#endif()

#查找boost库
find_package(Boost COMPONENTS thread filesystem locale system)

if(BOOST_FOUND)
    message("-------------------boost found---------------")
    message("---------------------------------------------")
    message("Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
    message("Boost_LIBRARIES = ${Boost_LIBRARIES}.")
    message("Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
    message("Boost_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}.")
    message("---------------------------------------------")
else()
    message("-------------------boost not found---------------")
    return()
endif()

#设置不适用系统的的boost的库
set(BOOST_NO_SYSTEM_PATHS ON)

#设置使用静态的boost库
set(Boost_USE_STATIC_LIBS ON)

add_executable (${target_name} "main.cpp")

# 不知道为啥boost不能用target_include_directories target_link_directories
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
target_link_libraries (${target_name} ${Boost_LIBRARIES})

install(TARGETS ${target_name} RUNTIME DESTINATION ./)