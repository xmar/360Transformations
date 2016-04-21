set (CMAKE_CXX_STANDARD 11)

find_package( FFMPEG REQUIRED )
MESSAGE( STATUS "FFMPEG found:         " ${FFMPEG_FOUND} )

include_directories(inc ${FFMPEG_INCLUDE_DIR})


FILE(GLOB LibAvWrapperSrc src/*.cpp)

add_library(LibAvWrapper ${LibAvWrapperSrc})
target_compile_features(LibAvWrapper PRIVATE cxx_range_for cxx_nullptr cxx_auto_type)
target_link_libraries( LibAvWrapper  ${FFMPEG_LIBRARIES} )

target_include_directories(LibAvWrapper PUBLIC inc)
