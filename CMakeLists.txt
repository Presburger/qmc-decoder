cmake_minimum_required(VERSION 2.9)
project(qmc-decoder)

# Judge the compiler to use different parameters
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++17")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(${Boost_INCLUDE_DIRS})
aux_source_directory(src SRC)
add_executable(decoder ${SRC})
if (UNIX)
    target_link_libraries(decoder pthread)
endif (UNIX)
