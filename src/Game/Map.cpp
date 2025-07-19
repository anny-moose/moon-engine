#include "Map.h"
#include <fstream>
#include <iostream>
#include <sstream>



bool GameMap::load_walls_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    walls.clear();
    std::string line;

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find("//") == 0)
            continue;

        std::istringstream iss(line);
        std::string token;
        std::vector<float> values;

        while (std::getline(iss, token, ',')) {
            try {
                values.push_back(std::stof(token)*50);
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid number in line: " << line << std::endl;
                return false;
            }
        }

        if (values.size() != 4) {
            std::cerr << "Invalid format in line: " << line << std::endl;
            return false;
        }

        Wall wall;
        wall.bound.x = values[0];
        wall.bound.y = values[1];
        wall.bound.width = values[2];
        wall.bound.height = values[3];
        walls.push_back(wall);
    }

    return true;
}

bool GameMap::load_triggers_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    triggers.clear();
    std::string line;

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find("//") == 0)
            continue;


        size_t pos = line.find("@@");
        std::string numberPart = line.substr(0, pos);
        std::string entityName = (pos != std::string::npos) ? line.substr(pos + 2) : "";

        std::istringstream iss(line);
        std::string token;
        std::vector<float> values;

        while (std::getline(iss, token, ',')) {
            try {
                values.push_back(std::stof(token)*50);
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid number in line: " << line << std::endl;
                return false;
            }
        }

        if (values.size() != 4) {
            std::cerr << "Invalid format in line: " << line << std::endl;
            return false;
        }

        TriggerArea area;
        area.bound.x = values[0];
        area.bound.y = values[1];
        area.bound.width = values[2];
        area.bound.height = values[3];
        if (!entityName.empty()) {
            area.action = Action::FOCUS_CAMERA;
            area.action_opt = entityName;
        }
        triggers.push_back(area);
    }

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
