#pragma once
#include "Light.h"
#include "raylib.h"
#include "../Entity/Entity.h"
#include "../Entity/Bullet.h"
#include "Map.h"
#include "UI/UIElement.h"
#include <algorithm>

typedef enum {
    MENU = 0,
    RUNNING,
    PLAYER_DEAD,
    GAME_WON,
} GameState;


class Game {
private:
    Entity player;
    const Entity *focused_entity = &player;

    Container main_menu = Container({200, 200, 700, 100}, VERTICAL);

    BulletManager bullet_manager;
    std::vector<Entity> enemies;

    std::vector<Entity> npcs;

    GameMap map;
    GameState state = MENU;


    NPCBehavior *npc_behavior = nullptr;

    Camera2D camera = {0};

    Light* player_light = nullptr;

    bool game_should_close = false;

public:
    explicit Game() : player(Entity(nullptr, {0, 0})) {
        Entity::manager = &bullet_manager;
        Entity::game = this;
        UIElement::game = this;
        EntityBehavior::player = &player;
        PlayerBehavior::camera = &camera;

        main_menu.emplace_back<Button>(Rectangle{240, 120, 240, 120}, [](Game &game) { game.set_game_state(RUNNING); },
                                       "play game");
        main_menu.emplace_back<Button>(Rectangle{240, 120, 240, 120}, [](Game &game) { game.set_game_state(GAME_WON); },
                                       "family-friendly button");
        main_menu.emplace_back<Button>(Rectangle{240, 120, 240, 120}, [](Game &game) { game.close_window(); },
                                       "exit game");

    }

    void close_window() {
        game_should_close = true;
    }

    void focus_entity(const std::string &entity_id);
    void turn_npc_into_enemy(Entity& npc) {
        npc.set_behavior(std::make_unique<EnemyBehavior>());
        auto it = std::find(npcs.begin(), npcs.end(), npc);
        if (it != npcs.end()) {
            enemies.push_back(std::move(npc));
            npcs.erase(it);
        }
    }

    bool load_entities_from_file(std::string file_path);

    void set_game_state(GameState new_state) { state = new_state; }
    void set_dialogue(NPCBehavior *npc) { npc_behavior = npc; }

    int run();

    void game_loop();

    void render_step();
};
