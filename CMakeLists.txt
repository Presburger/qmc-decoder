cmake_minimum_required(VERSION 2.9)
project(qmc-decoder)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11")
aux_source_directory(src SRC)
add_executable(decoder ${SRC})


