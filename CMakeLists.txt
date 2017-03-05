cmake_minimum_required(VERSION 3.6)
project(socker)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_VERBOSE_MAKEFILE on )
set(SOURCE_FILES socker.cpp rasocket.h send.h send.cpp usage.cpp echo.cpp usage.h)
add_executable(socker ${SOURCE_FILES} echo.h usage.h usage.cpp)