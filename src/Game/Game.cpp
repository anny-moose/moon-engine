#include "Game.h"
#include <iostream>
#include <fstream>
#include "UI/UIElement.h"
#include "../../lib/json.hpp"
#include "Light.h"
using json = nlohmann::json;

void Game::focus_entity(const std::string &entity_id) {
    for (const auto &entity: enemies) {
        if (entity_id == entity.get_id()) {
            focused_entity = &entity;
        }
    }
    if (entity_id == player.get_id())
        focused_entity = &player;

    for (const auto &npc: npcs)
        if (entity_id == npc.get_id())
            focused_entity = &npc;
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

    Light light = Light{player.get_position().x, player.get_position().y, 2000.0f, camera};
    player_light = &light;

    camera.target = {
                                                                                     focused_entity->get_position().x + focused_entity->get_size().x / 2,
                                                                                     focused_entity->get_position().y + focused_entity->get_size().y / 2
                                                                                 };
    camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.zoom = 1.0f;

    player_light->MoveLight(player.get_position().x, player.get_position().y);

    HideCursor();

    game_loop();

    player_light->UnloadRenderMask();

    CloseWindow();
    return 0;
}

void Game::game_loop() {
    Vector2 resolution = {(float)GetRenderWidth(), (float)GetRenderHeight()};
    Vector2 current_res = resolution;
    bool resolution_changed = false;
    while (!game_should_close && !WindowShouldClose()) {
        // Events
        resolution_changed = false;
        current_res = {(float)GetRenderWidth(), (float)GetRenderHeight()};
        if (current_res != resolution) {
            resolution = current_res;
            resolution_changed = true;
        }

        if (state == RUNNING) {
            player.move(map);
            for (auto &enemyIt: enemies) {
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
            bullet_manager.logic_tick(map.get_walls(), enemies, player);
            for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
                if (!enemyIt->tick(map))
                    enemyIt = enemies.erase(enemyIt);
                else
                    ++enemyIt;
            }

            for (auto &npc: npcs) {
                npc.move(map);
                if (!npc.tick(map)) {
                    npc_behavior = nullptr;
                    focus_entity(player.get_id());
                }
            }
        }


        if (state == MENU) {
            if (resolution_changed)
                main_menu.set_bounds(Rectangle{static_cast<float>(GetScreenWidth())/4, static_cast<float>(GetScreenHeight())/4, static_cast<float>(GetScreenWidth())/2, static_cast<float>(GetScreenHeight())/2});
            main_menu.update_element();
        }

        if (enemies.empty()) state = GAME_WON;

        const float camera_smoothness = 0.1f;
        camera.target +=
        {
            ((focused_entity->get_position().x + focused_entity->get_size().x / 2) -
             camera.target.x) *
            camera_smoothness,
            ((focused_entity->get_position().y + focused_entity->get_size().y / 2) -
             camera.target.y) *
            camera_smoothness
        };
        camera.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};

        player_light->UpdateLight(map.get_walls(), resolution_changed);
        player_light->MoveLight(player.get_position().x+(player.get_size().x/2), player.get_position().y+(player.get_size().y/2));

        render_step();

    }
}

static void draw_crosshair(const Entity &player) {
    const float reload_offset = 2;
    float size_adj = 1 + player.get_reload_clock()*reload_offset;
    float size = 12 * ((size_adj >= 1) ? size_adj : 1);

    DrawRectangle(GetMousePosition().x - size/2, GetMousePosition().y - size/2, size, size, WHITE);
}

void Game::render_step() {
    BeginDrawing();
    ClearBackground(SKYBLUE);

    if (state == MENU) {
        main_menu.draw_element();
    } else if (state == RUNNING) {
        BeginMode2D(camera);

        DrawText("Hello, Raylib!", 350, 280, 20, DARKGRAY);

        player.draw();

        for (const auto &bullet: *bullet_manager.get_bullets()) {
            DrawRectanglePro(bullet.get_hitbox(), {0, 0}, 0, WHITE);
        }

        for (const auto &enemy: enemies) {
            enemy.draw();
        }

        for (const auto &npc: npcs)
            npc.draw();

        // for (const auto &wall: map.get_walls()) {
        //     DrawRectanglePro(wall.bound, {0, 0}, 0, BLUE);
        // }


        player_light->RenderLightMask();
        EndMode2D();

        if (npc_behavior != nullptr)
            npc_behavior->draw_dialogue();
    } else if (state == PLAYER_DEAD) {
        DrawText("you losar", 350, 280, 80, DARKGRAY);
    } else if (state == GAME_WON) {
        DrawText("you winrar", 350, 280, 80, DARKGRAY);
    }

    draw_crosshair(player);

    EndDrawing();
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

    enemies.clear();

    for (auto it: data["entities"])
        enemies.emplace_back(Entity(
            std::make_unique<EnemyBehavior>((it["type"] == "warding") ? EnemyType::WARDEN : EnemyType::NORMAL), {
                it["position"][0].template get<float>() * 50, it["position"][1].template get<float>() * 50
            }, it["id"], DEFAULT_ENTITY_SIZE_PX, it["health"]));

    for (auto it : data["npcs"]) {
        std::vector<std::pair<std::string, std::string>> dialogue;
        for (auto entry : it["dialogue"]) {
            dialogue.push_back({entry["who"], entry["what"]});
        }

        npcs.emplace_back(Entity(std::make_unique<NPCBehavior>(dialogue), {it["position"][0].template get<float>() * 50, it["position"][1].template get<float>() * 50},
            it["id"], DEFAULT_ENTITY_SIZE_PX, 5, 10, 50));
    }

    return true;
}
