cmake_minimum_required(VERSION 3.0)
project(ShotDetection)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )

add_executable(ShotDetect ShotDetect.cpp)
add_executable(ShotDetect2 ShotDetect2.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries( ShotDetect ${OpenCV_LIBS} )
target_link_libraries( ShotDetect2 ${OpenCV_LIBS} )