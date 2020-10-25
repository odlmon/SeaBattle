cmake_minimum_required(VERSION 3.15)
project(SeaBattle)

set(CMAKE_CXX_STANDARD 11)

add_executable(SeaBattle WIN32 main.cpp)
target_link_libraries(SeaBattle Gdiplus.lib)