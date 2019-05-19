cmake_minimum_required(VERSION 2.9)
project(qmc-decoder)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -std=c++11")
if(NOT APPLE)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

set(Boost_USE_STATIC_LIBS  ON)
find_package(Boost 1.56 REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
aux_source_directory(src SRC)
add_executable(decoder ${SRC})
target_link_libraries(decoder ${Boost_LIBRARIES})



