cmake_minimum_required(VERSION 3.6)
project(socker)

set (CLION 1)
#set(CMAKE_CXX_STANDARD 11)
set( CMAKE_VERBOSE_MAKEFILE on )
set(SOURCE_FILES socker.c rasocket.h send.h send.c usage.c echo.c usage.h)
add_executable(socker ${SOURCE_FILES} echo.h usage.h usage.c proxy.c proxy.h utils.c utils.h oneTimeBuffer.c oneTimeBuffer.h testOneTimeBuffer.c testOneTimeBuffer.h testSessionHandler.c testSessionHandler.h sessionHandler.c sessionHandler.h multi_proxy.c multi_proxy.h)