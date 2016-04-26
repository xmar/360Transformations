cmake_minimum_required( VERSION 3.1.0 FATAL_ERROR )
project( trans )

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(LibAvWrapper)
add_subdirectory(MainProject)
