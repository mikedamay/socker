cmake_minimum_required(VERSION 3.6)
project(socker)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_VERBOSE_MAKEFILE on )
set(SOURCE_FILES socker.cpp rasocket.h connect.h connect.cpp usage.cpp bind.cpp usage.h)
add_executable(socker ${SOURCE_FILES} bind.h usage.h usage.cpp)