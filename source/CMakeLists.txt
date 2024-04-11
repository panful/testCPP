
include(${PROJECT_SOURCE_DIR}/cmake/build_target.cmake)

recursive_add_executables(${CMAKE_CURRENT_SOURCE_DIR}/01_cpp "01")
recursive_add_executables(${CMAKE_CURRENT_SOURCE_DIR}/02_modern_cpp "02")
recursive_add_executables(${CMAKE_CURRENT_SOURCE_DIR}/03_stl "03")
recursive_add_executables(${CMAKE_CURRENT_SOURCE_DIR}/04_data_structure "04")

#add_subdirectory("05_network")
#add_subdirectory("06_other")
