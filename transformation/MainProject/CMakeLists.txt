set (CMAKE_CXX_STANDARD 11)

find_package(OpenMP QUIET)                                         
if(OPENMP_FOUND)                                                      
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")            
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")         
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

if (USE_CONAN)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else()
    find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
    find_package(OpenCV REQUIRED)
endif()

include_directories( inc )

FILE(GLOB MainSrc src/*.cpp)

add_executable( trans ${MainSrc})
target_compile_features(trans PRIVATE cxx_range_for)
target_link_libraries( trans ${CONAN_LIBS} ${Boost_LIBRARIES} ${OpenCV_LIBS} LibAvWrapper )
#target_link_libraries( trans ${Boost_LIBRARIES} ${OpenCV_LIBS} LibAvWrapper )
#add_custom_command(TARGET trans POST_BUILD COMMAND cp MainProject/trans ..)
