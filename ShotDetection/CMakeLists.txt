cmake_minimum_required(VERSION 3.0)
project(ShotDetection)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )

add_executable(ShotDetect ShotDetect.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries( ShotDetect ${OpenCV_LIBS} )