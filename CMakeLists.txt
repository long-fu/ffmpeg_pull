cmake_minimum_required(VERSION 3.0.0)
project(pull_stream VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include_directories(
    ./inc/
    /usr/local/include/
)
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( "/usr/local/include/opencv4" )

add_executable(pull_stream 
src/pull_stream.cpp
main.cpp)
target_link_libraries(pull_stream stdc++ pthread avcodec avutil swscale avformat avcodec avformat avdevice avutil swresample avfilter swscale ${OpenCV_LIBS})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
