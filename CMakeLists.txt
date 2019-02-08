cmake_minimum_required(VERSION 2.8.3)
project(bebop2)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  geometry_msgs
)

catkin_package(
  INCLUDE_DIRS src
  CATKIN_DEPENDS 
  roscpp 
  geometry_msgs
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(bebop2_teleop src/bebop2_teleop.cpp)
target_link_libraries(bebop2_teleop ${catkin_LIBRARIES})

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
