cmake_minimum_required(VERSION 3.31)
project(raylib_game)

find_package(raylib 5.5 REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(raylib_game main.cpp
        Map.h
        Map.cpp)

target_link_libraries(${PROJECT_NAME} raylib)