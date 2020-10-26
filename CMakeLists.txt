cmake_minimum_required(VERSION 3.15)
project(SeaBattle)

set(CMAKE_CXX_STANDARD 11)

add_executable(SeaBattle WIN32 main.cpp)
target_link_libraries(SeaBattle Gdiplus.lib handlers)

add_library(handlers handlers.cpp handlers.h)
target_link_libraries(handlers state_changers)

add_library(state_changers state_changers.cpp state_changers.h)