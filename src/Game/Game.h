#pragma once
#include "raylib.h"
#include "../Entity/Entity.h"
#include "../Entity/Bullet.h"
#include "Map.h"

typedef enum {
    MENU = 0,
    RUNNING,
    DIALOGUE,
    PLAYER_DEAD,
    GAME_WON,
} GameState;


class Game {
private:
    Entity player;
    Entity *focused_entity = &player;

    BulletManager bullet_manager;
    std::vector<Entity> npcs;

    GameMap map;
    GameState state = RUNNING;

    Camera2D camera = { 0 };

public:
    explicit Game() : player(Entity(nullptr, {0, 0})) {
        Entity::manager = &bullet_manager;
        Entity::game = this;
        EnemyBehavior::player = &player;
        PlayerBehavior::camera = &camera;
    }

    void focus_entity(std::string entity_id);

    bool load_entities_from_file(std::string file_path);

    int run();
    void game_loop();
};