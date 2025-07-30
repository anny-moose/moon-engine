#include "Map.h"

#include <fstream>
#include <iostream>
#include <raymath.h>

#include "../../lib/json.hpp"
using json = nlohmann::json;


bool GameMap::load_map_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    json data = json::parse(file);

    walls.clear();
    triggers.clear();

    for (auto it: data["walls"]) {
        walls.push_back(Wall{
            it["pos_x"].template get<float>() * 50, it["pos_y"].template get<float>() * 50,
            it["width"].template get<float>() * 50, it["height"].template get<float>() * 50
        });
    }

    for (auto it: data["triggers"]) {
        TriggerArea a = {
            it["pos_x"].template get<float>() * 50, it["pos_y"].template get<float>() * 50,
            it["width"].template get<float>() * 50, it["height"].template get<float>() * 50
        };

        a.action = (it["action"] == "focus_cam") ? Action::FOCUS_CAMERA : Action::FREEZE_PLAYER;
        a.action_opt = it["action_opt"];

        triggers.push_back(a);
    }

    return true;
}

bool Wall::CheckCollisionLineDiag(Vector2 p1, Vector2 p2) const {
    std::pair<std::pair<Vector2, Vector2>, std::pair<Vector2, Vector2> > wall_lines = {
        {{bound.x, bound.y}, {bound.x + bound.width, bound.y + bound.height}},
        {{bound.x + bound.width, bound.y}, {bound.x, bound.y + bound.height}}
    };


    if (CheckCollisionLines(p1, p2, wall_lines.first.first, wall_lines.first.second, nullptr)) return true;
    if (CheckCollisionLines(p1, p2, wall_lines.second.first, wall_lines.second.second, nullptr)) return true;


    return false;
}
