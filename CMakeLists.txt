cmake_minimum_required(VERSION 2.8)
project(stbresize)
add_compile_options(-O3)
include_directories(src)
add_executable(stbresize src/stbresize.c src/dependencies.c)
target_link_libraries(stbresize m)
