cmake_minimum_required(VERSION 3.0)
project(simpleQuery)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )

add_executable(HSVquery HSVquery.cpp)
add_executable(Conceptquery Conceptquery.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries( HSVquery ${OpenCV_LIBS} sqlite3)
target_link_libraries( Conceptquery ${OpenCV_LIBS} sqlite3)