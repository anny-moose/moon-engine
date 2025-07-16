#include "Map.h"
#include <iostream>

// FIXME: this code is FUCKING ABHORRENT.


void load_map_from_file(GameMap& map, std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    map.walls.clear();

    std::pair<Vector2, Vector2> wall;
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
                wall.first.x = negated ? -currentNumber : currentNumber;
            else
                wall.second.x = negated ? -currentNumber : currentNumber;
            currentNumber = 0;
            negated = false;
        } else if (ch == ';') {
            wall.first.y = negated ? -currentNumber : currentNumber;
            currentNumber = 0;
            negated = false;
            secondRun = true;
        } else {
            wall.second.y = negated ? -currentNumber : currentNumber;
            currentNumber = 0;
            secondRun = false;
            negated = false;
            map.walls.push_back(wall);
        }
    }

    if (secondRun) {
        wall.second.y = negated ? -currentNumber : currentNumber;
        map.walls.push_back(wall);
    }

    file.close();
}

void load_trigger_areas(GameMap& map, std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    map.trigger_areas.clear();

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
                area.area.first.x = negated ? -currentNumber : currentNumber;
            else
                area.area.second.x = negated ? -currentNumber : currentNumber;
            currentNumber = 0;
            negated = false;
        } else if (ch == ';') {
            area.area.first.y = negated ? -currentNumber : currentNumber;
            currentNumber = 0;
            negated = false;
            secondRun = true;
        } else if (ch == '@') {
            area.action = Action::FREEZE_PLAYER;
        } else if (ch == '!') {
            area.action = Action::FOCUS_CAMERA;
            readingCamFocus = true;
        } else {
            area.area.second.y = negated ? -currentNumber : currentNumber;
            currentNumber = 0;
            negated = false;
            secondRun = false;
            map.trigger_areas.push_back(area);
            area.action_opt.clear();
        }
    }

    if (secondRun) {
        area.area.second.y = negated ? -currentNumber : currentNumber;
        map.trigger_areas.push_back(area);
    }

    file.close();
}


void load_entities_from_file(GameMap &map, std::string file_path) {
    std::ifstream file(file_path);

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    map.enemies.clear();

    Entity entity_temp;

    bool firstRun = true;
    bool negated = false;
    bool comment = false;

    bool readingName = false;
    std::string name;

    int currentNumber = 0;


    char ch;
    while (file.get(ch)) {
        if (comment) {
            if (ch != '/') {
                continue;
            }
            comment = false;
            continue;
        }

        if (readingName) {
           if (ch != '@') {
               if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
                   name += ch;
               continue;
           }
            readingName = false;
        }

        else if (ch == '-') {
            negated = true;
        } else if (ch >= '0' && ch <= '9') {
            currentNumber = currentNumber * 10 + ch - '0';
        } else if (ch == ',') {
            entity_temp.position.x = negated ? -currentNumber*50 : currentNumber*50;
            currentNumber = 0;
            negated = false;
        } else if (ch == ';') {
            entity_temp.position.y = negated ? -currentNumber*50 : currentNumber*50;
            currentNumber = 0;
            negated = false;
        } else if (ch == '/') comment = true;
        else if (ch == '@') {
            readingName = true;
        } else {
            entity_temp.size = {50, 50};
            entity_temp.health = currentNumber;
            entity_temp.reload_clock = 0.0f;
            entity_temp.reload_time = 0.50f;
            entity_temp.name = name;
            name.clear();
            if (firstRun) map.player = entity_temp;
            else map.enemies.push_back(entity_temp);
            entity_temp = {};
            currentNumber = 0;
            firstRun = false;
        }
    }
}
