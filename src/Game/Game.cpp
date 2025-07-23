#include "Game.h"
#include <iostream>
#include <fstream>
#include "UI/UIElement.h"
#include "../../lib/json.hpp"
using json = nlohmann::json;

void Game::focus_entity(const std::string &entity_id) {
    for (auto &entity: entities) {
        if (entity_id == entity.get_id()) {
            focused_entity = &entity;
        }
    }
    if (entity_id == player.get_id())
        focused_entity = &player;
}


int Game::run() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib C++ App");
    SetTargetFPS(240);

    map.load_map_from_file("map.json");
    load_entities_from_file("entities.json");

    camera.target = {
        focused_entity->get_position().x + focused_entity->get_size().x / 2,
        focused_entity->get_position().y + focused_entity->get_size().y / 2
    };
    camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.zoom = 1.0f;

    HideCursor();

    game_loop();


    CloseWindow();
    return 0;
}

void Game::game_loop() {
    while (!game_should_close && !WindowShouldClose()) {
        // Events

        if (state == RUNNING) {
            player.move(map);
            for (auto &enemyIt: entities) {
                enemyIt.move(map);
            }
        }

        if (IsKeyPressed(KEY_P)) {
            // freeze_player_toggle(map.player);
            //
            // focus_camera_on_entity(map, "enemy1");
        }

        // Logic

        if (state == RUNNING) {
            if (!player.tick(map)) set_game_state(PLAYER_DEAD);
            bullet_manager.logic_tick(map.get_walls(), entities, player);
            for (auto enemyIt = entities.begin(); enemyIt != entities.end();) {
                if (!enemyIt->tick(map))
                    enemyIt = entities.erase(enemyIt);
                else
                    ++enemyIt;
            }
        }

        if (state == MENU) {
            main_menu.set_bounds((Rectangle){10, (float)GetScreenHeight() - 200, (float)GetScreenWidth()-20, 190});
            main_menu.update_element();
        }

        if (entities.empty()) state = GAME_WON;

        const float smoothness = 0.1f;
        camera.target +=
        {
            ((focused_entity->get_position().x + focused_entity->get_size().x / 2) -
             camera.target.x) *
            smoothness,
            ((focused_entity->get_position().y + focused_entity->get_size().y / 2) -
             camera.target.y) *
            smoothness
        };
        camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);

        if (state == MENU) {
            main_menu.draw_element();
        } else if (state == RUNNING) {
            BeginMode2D(camera);

            DrawText("Hello, Raylib!", 350, 280, 20, DARKGRAY);

            DrawRectangle(player.get_position().x, player.get_position().y,
                          player.get_size().x, player.get_size().y,
                          (player.get_invulnerability_time() < 0) ? DARKGREEN : (Color){0, 77, 4, 255});
            DrawText(std::format("Health: {}", player.get_health()).c_str(),
                     player.get_position().x - 20, player.get_position().y - 20, 20, GREEN);

            for (const auto &bullet: *bullet_manager.get_bullets()) {
                DrawRectanglePro(bullet.get_hitbox(), {0, 0}, 0, WHITE);
            }

            for (const auto &enemy: entities) {
                DrawRectangle(enemy.get_position().x, enemy.get_position().y, enemy.get_size().x,
                              enemy.get_size().y,
                              (enemy.get_invulnerability_time() < 0) ? RED : (Color){190, 1, 15, 255});
                DrawText(std::format("Health: {}", enemy.get_health()).c_str(),
                         enemy.get_position().x - 20, enemy.get_position().y - 20, 20, RED);
            }

            for (const auto &wall: map.get_walls()) {
                DrawRectanglePro(wall.bound, {0, 0}, 0, BLUE);
            }

            EndMode2D();
        } else if (state == PLAYER_DEAD) {
            DrawText("you losar", 350, 280, 80, DARKGRAY);
        } else if (state == GAME_WON) {
            DrawText("you winrar", 350, 280, 80, DARKGRAY);
        }

        DrawRectangle(GetMousePosition().x - 12, GetMousePosition().y - 12, 24, 24, WHITE);

        EndDrawing();
    }
}


bool Game::load_entities_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    json data = json::parse(file);

    player = Entity(std::make_unique<PlayerBehavior>(), {
                        data["player"]["position"][0].template get<float>() * 50,
                        data["player"]["position"][1].template get<float>() * 50
                    }, "player");

    entities.clear();

    for (auto it: data["entities"])
        entities.emplace_back(Entity(
            std::make_unique<EnemyBehavior>((it["type"] == "warding") ? EnemyType::WARDEN : EnemyType::NORMAL), {
                it["position"][0].template get<float>() * 50, it["position"][1].template get<float>() * 50
            }, it["id"], DEFAULT_ENTITY_SIZE_PX, it["health"]));

    return true;
}
