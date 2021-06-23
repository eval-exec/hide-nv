cmake_minimum_required(VERSION 3.19)
project(hide_dlsym C)

set(CMAKE_C_STANDARD 11)

#include_directories("tdk_2.295.2/include")
#include_directories("tdk_2.295.2/include")

add_library(hide_dlsym SHARED hide_dlsym.c )
