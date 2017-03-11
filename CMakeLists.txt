cmake_minimum_required(VERSION 3.6)
project(socker)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_VERBOSE_MAKEFILE on )
set(SOURCE_FILES socker.cpp rasocket.h send.h send.cpp usage.cpp echo.cpp usage.h)
add_executable(socker ${SOURCE_FILES} echo.h usage.h usage.cpp proxy.cpp proxy.h utils.cpp utils.h oneTimeBuffer.cpp oneTimeBuffer.h testOneTimeBuffer.cpp testOneTimeBuffer.h testSessionHandler.cpp testSessionHandler.h sessionHandler.cpp sessionHandler.h multi_proxy.cpp multi_proxy.h)