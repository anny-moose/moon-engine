#pragma once
#include <fstream>
#include "raylib.h"
#include <vector>

typedef struct {
    std::vector<std::pair<Vector2, Vector2>> walls; // pos, size
} GameMap;

GameMap read_from_file(std::string file_path);