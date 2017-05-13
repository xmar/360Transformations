set (CMAKE_CXX_STANDARD 11)

LIST(APPEND CMAKE_MODULE_PATH "." "..")


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


FILE(GLOB LibAvWrapperSrc src/*.cpp)

add_library(LibAvWrapper ${LibAvWrapperSrc})
target_compile_features(LibAvWrapper PRIVATE cxx_range_for cxx_nullptr cxx_auto_type)
target_link_libraries( LibAvWrapper  ${CONAN_LIBS} )

target_include_directories(LibAvWrapper PUBLIC inc)
