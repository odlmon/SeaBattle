cmake_minimum_required(VERSION 3.15)
project(SeaBattle)

set(CMAKE_CXX_STANDARD 11)

add_executable(SeaBattle WIN32 main/main.cpp resources/resources.rc resources/main_menu.rc)
target_link_libraries(SeaBattle handlers_pregame handlers_game)

add_library(handlers_pregame pregame/handlers/handlers.cpp pregame/handlers/handlers.h)
target_link_libraries(handlers_pregame state_changers_pregame state_changers_common handlers_common)

add_library(state_changers_pregame pregame/state_changers/state_changers.cpp pregame/state_changers/state_changers.h)

add_library(handlers_game game/handlers/handlers.cpp game/handlers/handlers.h)
target_link_libraries(handlers_game state_changers_game state_changers_common handlers_common)

add_library(state_changers_game game/state_changers/state_changers.cpp game/state_changers/state_changers.h)
target_link_libraries(state_changers_game winmm.lib)

add_library(handlers_common common/handlers/handlers.cpp common/handlers/handlers.h)

add_library(state_changers_common common/state_changers/state_changers.cpp common/state_changers/state_changers.h)