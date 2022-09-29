cmake_minimum_required(VERSION 3.8)

if(WIN32)
    add_subdirectory("01_Socket")
endif()
add_subdirectory("02_Http")
add_subdirectory("03_Rpc")