cmake_minimum_required( VERSION 3.1.0 FATAL_ERROR )
project( trans )

set (CMAKE_C_STANDARD 11)

LIST(APPEND CMAKE_MODULE_PATH ".")

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
find_package(OpenCV REQUIRED)

FILE(GLOB MySrc src/*.cpp)

add_executable( trans ${MySrc} )
target_compile_features(trans PRIVATE cxx_range_for)
target_link_libraries( trans ${Boost_LIBRARIES} ${OpenCV_LIBS})
