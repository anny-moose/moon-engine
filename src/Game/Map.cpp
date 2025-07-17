#include "Map.h"
#include <fstream>
#include <iostream>


constexpr int neg_check(bool cond, int x) {
    return cond ? -x * 50 : x * 50;
}

bool GameMap::load_walls_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    walls.clear();

    Wall wall;
    int currentNumber = 0;

    bool secondRun = false;
    bool negated = false;
    bool comment = false;

    char ch;
    while (file.get(ch)) {
        if (comment) {
            if (ch == '/') {
                comment = false;
            }
        } else if (ch == '/')
            comment = true;
        else if (ch == '-') {
            negated = true;
        } else if (ch >= '0' && ch <= '9') {
            currentNumber = currentNumber * 10 + ch - '0';
        } else if (ch == ',') {
            if (!secondRun)
                wall.bound.x = neg_check(negated, currentNumber) ;
            else
                wall.bound.width = neg_check(negated, currentNumber);
            currentNumber = 0;
            negated = false;
        } else if (ch == ';') {
            wall.bound.y = neg_check(negated, currentNumber);
            currentNumber = 0;
            negated = false;
            secondRun = true;
        } else {
            wall.bound.height = neg_check(negated, currentNumber);
            currentNumber = 0;
            secondRun = false;
            negated = false;
            walls.push_back(wall);
        }
    }

    if (secondRun) {
        wall.bound.height = neg_check(negated, currentNumber);
        walls.push_back(wall);
    }

    file.close();
    return true;
}

bool GameMap::load_triggers_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    triggers.clear();

    TriggerArea area;

    int currentNumber = 0;

    bool secondRun = false;
    bool negated = false;
    bool comment = false;
    bool readingCamFocus = false;

    char ch;
    while (file.get(ch)) {
        if (comment) {
            if (ch != '/') {
                continue;
            }
            comment = false;
            continue;
        }

        if (readingCamFocus) {
            if (ch != '!') {
                area.action_opt += ch;
                continue;
            }
            readingCamFocus = false;
            continue;
        }

        if (ch == '/') {
            comment = true;
        } else if (ch == '-') {
            negated = true;
        } else if (ch >= '0' && ch <= '9') {
            currentNumber = currentNumber * 10 + ch - '0';
        } else if (ch == ',') {
            if (!secondRun)
                area.bound.x = neg_check(negated, currentNumber);
            else
                area.bound.width = neg_check(negated, currentNumber);
            currentNumber = 0;
            negated = false;
        } else if (ch == ';') {
            area.bound.y = neg_check(negated, currentNumber);
            currentNumber = 0;
            negated = false;
            secondRun = true;
        } else if (ch == '@') {
            area.action = Action::FREEZE_PLAYER;
        } else if (ch == '!') {
            area.action = Action::FOCUS_CAMERA;
            readingCamFocus = true;
        } else {
            area.bound.height = neg_check(negated, currentNumber);
            currentNumber = 0;
            negated = false;
            secondRun = false;
            triggers.push_back(area);
            area.action_opt.clear();
        }
    }

    if (secondRun) {
        area.bound.height = neg_check(negated, currentNumber);
        triggers.push_back(area);
    }

    file.close();

    return true;
}

bool Wall::CheckCollisionLine(Vector2 p1, Vector2 p2) const {
    std::pair<std::pair<Vector2, Vector2>, std::pair<Vector2, Vector2>> wall_lines = {
    {{bound.x, bound.y}, {bound.x + bound.width, bound.y + bound.height}},
    {{bound.x + bound.width, bound.y}, {bound.x, bound.y + bound.height}}
    };


    if (CheckCollisionLines(p1, p2, wall_lines.first.first, wall_lines.first.second, nullptr)) return true;
    if (CheckCollisionLines(p1, p2, wall_lines.second.first, wall_lines.second.second, nullptr)) return true;


    return false;
}
