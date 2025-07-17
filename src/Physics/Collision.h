#pragma once
#include "raylib.h"
#include <utility>

void collision_x(const Rectangle &hitbox, float &vel_x, std::pair<float, float> wall_x);

void collision_y(const Rectangle &hitbox, float &vel_y, std::pair<float, float> wall_y);

bool CheckCollisionLineWall(std::pair<Vector2, Vector2> line1, std::pair<Vector2, Vector2> wall);
