cmake_minimum_required( VERSION 3.1.0 FATAL_ERROR )
project( trans )

LIST(APPEND CMAKE_MODULE_PATH "." "..")

add_subdirectory(LibAvWrapper)
add_subdirectory(MainProject)
