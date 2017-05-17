cmake_minimum_required( VERSION 3.1.0 FATAL_ERROR )
project( trans_test )

set(CMAKE_BUILD_TYPE Release)

add_subdirectory(../LibAvWrapper binary_dir)

set (CMAKE_CXX_STANDARD 11)

find_package(OpenMP QUIET)
if(OPENMP_FOUND)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories( ../MainProject/inc )

FILE(GLOB MainSrc ../MainProject/src/*.cpp)
list(REMOVE_ITEM MainSrc ../MainProject/src/main.cpp)

add_library( trans ${MainSrc})
target_compile_features(trans PRIVATE cxx_range_for)
target_link_libraries( trans ${CONAN_LIBS} LibAvWrapper )

FILE(GLOB MainTestSrc ../MainProject/test/*.cpp)

include_directories( ../MainProject/inc )

add_executable( trans_test ${MainTestSrc})
target_compile_features(trans_test PRIVATE cxx_range_for)
target_link_libraries( trans_test ${CONAN_LIBS} trans )

add_custom_command(TARGET trans_test POST_BUILD COMMAND cp bin/trans_test .)
