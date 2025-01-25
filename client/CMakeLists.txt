cmake_minimum_required(VERSION 3.30)
project(client)

set(CMAKE_CXX_STANDARD 20)

add_executable(client main.cpp)

target_link_libraries(client ws2_32)
