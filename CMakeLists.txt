cmake_minimum_required(VERSION 2.8)
PROJECT(visualDemo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -ggdb -g")
add_definitions(-std=c++11) # Use C++11

find_package(raylib REQUIRED)

#Include directories
include_directories(
     ${CMAKE_SOURCE_DIR}
)
############## YDLIDAR SDK START######  ###############################
#find ydlidar_sdk package
find_package(ydlidar_sdk REQUIRED)
#Include directories
include_directories(
  ${YDLIDAR_SDK_INCLUDE_DIRS}
)

#link library directories
link_directories(${YDLIDAR_SDK_LIBRARY_DIRS})

add_executable(visualDemo visualDemo.cpp)
add_executable(scanExporter scanExporter.cpp)

#Link your project to ydlidar_sdk library.
target_link_libraries(${PROJECT_NAME} ${YDLIDAR_SDK_LIBRARIES})
target_link_libraries(scanExporter ${YDLIDAR_SDK_LIBRARIES})

############## YDLIDAR SDK END#####################################

target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(scanExporter raylib)