cmake_minimum_required(VERSION 3.13)
project(WebServer_Nick)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-std=c++11 -Wall -g -O0)
#add_compile_options(-std=c++11 -O3)

add_subdirectory(src)
