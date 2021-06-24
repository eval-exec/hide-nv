cmake_minimum_required(VERSION 3.19)
project(hide_dlsym)

set(CMAKE_C_STANDARD 11)

#include_directories("tdk_2.295.2/include")
#include_directories("tdk_2.295.2/include")

#target_link_libraries(hide_dlsym ${CMAKE_DL_LIBS})
#add_library(hide_dlsym SHARED processhider.c hide_dlsym.c)
add_library(hide_dlsym SHARED processhider.c)
#