set (CMAKE_CXX_STANDARD 11)

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
find_package(OpenCV 3.1 REQUIRED)

find_package(OpenMP QUIET)                                         
if(OPENMP_FOUND)                                                      
   MESSAGE( STATUS "xc: OpenMP found" )                               
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")            
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")         
   set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories( inc ${Boost_INCLUDE_DIR}) 

FILE(GLOB MainSrc src/*.cpp)

add_executable( trans ${MainSrc})
target_compile_features(LibAvWrapper PRIVATE cxx_range_for)
target_link_libraries( trans ${Boost_LIBRARIES} ${OpenCV_LIBS} LibAvWrapper )
