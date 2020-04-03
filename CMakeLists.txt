cmake_minimum_required(VERSION 2.9)
project(qmc-decoder)

set(CMAKE_FIND_ROOT_PATH "${PROJECT_SOURCE_DIR}/usr")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -std=c++11")
if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Android")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif()
find_package(Boost 1.56 REQUIRED COMPONENTS
             filesystem)
include_directories(${Boost_INCLUDE_DIRS})
aux_source_directory(src SRC)
add_executable(decoder ${SRC})
target_link_libraries(decoder Boost::filesystem)

