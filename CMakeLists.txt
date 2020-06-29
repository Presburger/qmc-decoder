cmake_minimum_required(VERSION 2.9)
project(qmc-decoder)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -std=c++11")
endif()

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -pthread -static-libgcc -static-libstdc++")
endif()

include_directories(3rdparty/filesystem/include)
aux_source_directory(src SRC)
add_executable(decoder ${SRC})

