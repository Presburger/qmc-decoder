cmake_minimum_required(VERSION 2.9)
project(qmc-decoder)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++14 -static-libgcc -static-libstdc++")
find_package(Boost 1.56 REQUIRED COMPONENTS
             filesystem)

include_directories(${Boost_INCLUDE_DIRS})
aux_source_directory(src SRC)
add_executable(decoder ${SRC})
target_link_libraries(decoder Boost::filesystem)

