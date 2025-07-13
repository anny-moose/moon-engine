#include "Map.h"
#include <iostream>

GameMap read_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return {{{{0, 0}, {0, 0}}}};
    }

    GameMap map;

    std::pair<Vector2, Vector2> wall;
    int currentNumber = 0;

    bool secondRun = false;
    bool negated = false;

    char ch;
    while (file.get(ch)) {
        if (ch == '-') {
            negated = true;
        } else if (ch >= '0' && ch <= '9') {
            currentNumber = currentNumber * 10 + ch - '0';
            if (negated) {
                currentNumber = -currentNumber;
                negated = false;
            }
        } else if (ch == ',') {
            if (!secondRun)
                wall.first.x = currentNumber;
            else
                wall.second.x = currentNumber;
            currentNumber = 0;
        } else if (ch == ';') {
            wall.first.y = currentNumber;
            currentNumber = 0;
            secondRun = true;
        } else {
            wall.second.y = currentNumber;
            currentNumber = 0;
            secondRun = false;
            map.walls.push_back(wall);
        }
    }

    if (secondRun) {
        wall.second.y = currentNumber;
        map.walls.push_back(wall);
    }

    file.close();

    return map;
}