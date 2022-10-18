cmake_minimum_required (VERSION 3.8)

set(target_name "06_02_MPI")

if(WIN32)
    set(MPI_DIR "C:/Program Files (x86)/Microsoft SDKs/MPI")
endif()

find_package(MPI)

if(MPI_CXX_FOUND)
    add_executable (${target_name} "main.cpp")

    target_include_directories(${target_name} PRIVATE ${MPI_CXX_INCLUDE_DIRS})
    target_link_libraries(${target_name} ${MPI_CXX_LIBRARIES})

    install(TARGETS ${target_name} RUNTIME DESTINATION ./)
else()
	message("===== MPI was not found =====")
endif()
