#pragma once
#include "raylib.h"
#include "../Entity/Entity.h"
#include "../Entity/Bullet.h"
#include "Map.h"
#include "UI/UIElement.h"

typedef enum {
    MENU = 0,
    RUNNING,
    PLAYER_DEAD,
    GAME_WON,
} GameState;


class Game {
private:
    Entity player;
    Entity *focused_entity = &player;

    ButtonContainer main_menu = ButtonContainer({200, 200, 700, 100});

    BulletManager bullet_manager;
    std::vector<Entity> entities;

    GameMap map;
    GameState state = MENU;

    Camera2D camera = { 0 };

    bool game_should_close = false;

public:
    explicit Game() : player(Entity(nullptr, {0, 0})) {
        Entity::manager = &bullet_manager;
        Entity::game = this;
        UIElement::game = this;
        EnemyBehavior::player = &player;
        PlayerBehavior::camera = &camera;

        main_menu.emplace_back(Button({240, 120, 240, 120}, [](Game& game){game.set_game_state(RUNNING);}, "play game"));
        main_menu.emplace_back(Button({240, 120, 240, 120}, [](Game& game){game.close_window();}, "exit game"));
        main_menu.emplace_back(Button({240, 120, 240, 120}, [](Game& game){game.set_game_state(GAME_WON);}, "fuck you"));
    }

    void close_window() {
        game_should_close = true;
    }

    void focus_entity(const std::string& entity_id);

    bool load_entities_from_file(std::string file_path);

    void set_game_state(GameState new_state) {state = new_state;}

    int run();
    void game_loop();
};