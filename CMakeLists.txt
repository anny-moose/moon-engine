cmake_minimum_required(VERSION 3.31)
project(raylib_game)

find_package(raylib 5.5 REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(raylib_game src/main
            src/Game/Game.cpp src/Game/Map.cpp
            src/Game/UI/UIElement.cpp
            src/Entity/Bullet.cpp src/Entity/Entity.cpp src/Entity/EnemyBehavior.cpp src/Entity/PlayerBehavior.cpp
            src/Physics/Collision.cpp
            src/main.cpp)

target_link_libraries(${PROJECT_NAME} raylib)