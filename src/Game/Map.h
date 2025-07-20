#pragma once
#include "raylib.h"
#include <vector>
#include <string>

typedef struct Wall {
    Rectangle bound;
    bool CheckCollisionLine(Vector2 p1, Vector2 p2) const;
} Wall;


typedef enum {
    FREEZE_PLAYER = 0,
    TRIGGER_DIALOGUE,
    FOCUS_CAMERA
} Action;


typedef struct {
    Rectangle bound;
    Action action;
    std::string action_opt;
} TriggerArea;



class GameMap {
private:
    std::vector<Wall> walls;
    std::vector<TriggerArea> triggers;
public:
    const std::vector<Wall>& get_walls() const { return walls; }
    std::vector<TriggerArea>& get_triggers() { return triggers; }

    bool load_map_from_file(std::string file_name);
};