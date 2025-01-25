cmake_minimum_required(VERSION 3.30)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server main.cpp)

target_link_libraries(server ws2_32)
