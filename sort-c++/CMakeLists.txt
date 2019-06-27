cmake_minimum_required(VERSION 3.5)
project(sort_cpp)

set(CMAKE_CXX_STANDARD 11)

# OpenCV
find_package(OpenCV REQUIRED)
MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")

# Build the library
add_library(sortcpp SHARED Hungarian.h Hungarian.cc KalmanTracker.h KalmanTracker.cc OnlineSortTracker.h OnlineSortTracker.cc)
target_link_libraries(sortcpp ${OpenCV_LIBS})

# Example
add_executable(main main.cc)
target_link_libraries(main sortcpp)

add_executable(main2 main2.cc)
target_link_libraries(main2 sortcpp)
