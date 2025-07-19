#include "Game.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Game::focus_entity(std::string entity_id) {
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib C++ App");
    SetTargetFPS(60);

    map.load_walls_from_file("map.txt");
    map.load_triggers_from_file("areas.txt");
    load_entities_from_file("entities.txt");

    camera.target = {
        focused_entity->get_position().x + focused_entity->get_size().x / 2,
        focused_entity->get_position().y + focused_entity->get_size().y / 2
    };
    camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.zoom = 1.0f;


    game_loop();


    CloseWindow();
    return 0;
}

void Game::game_loop() {

    while (!WindowShouldClose()) {
        // Events
        player.move(map);

        if (IsKeyPressed(KEY_P)) {
            // freeze_player_toggle(map.player);
            //
            // focus_camera_on_entity(map, "enemy1");
        }

        // Logic

        if (state == GameState::RUNNING) {
            if (!player.tick(map)) state = GameState::PLAYER_DEAD;
            bullet_manager.logic_tick(map.get_walls(), entities, player);
            for (auto enemyIt = entities.begin(); enemyIt != entities.end();) {
                enemyIt->move(map);
                if (!enemyIt->tick(map))
                    enemyIt = entities.erase(enemyIt);
                else
                    ++enemyIt;
            }
        }

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

        if (state == GameState::RUNNING) {
            BeginMode2D(camera);

            DrawText("Hello, Raylib!", 350, 280, 20, DARKGRAY);

            DrawRectangle(player.get_position().x, player.get_position().y,
                          player.get_size().x, player.get_size().y, DARKGREEN);
            DrawText(std::format("Health: {}", player.get_health()).c_str(),
                     player.get_position().x - 20, player.get_position().y - 20, 20, GREEN);

            for (const auto &bullet: *bullet_manager.get_bullets()) {
                DrawRectanglePro(bullet.get_hitbox(), {0, 0}, 0, WHITE);
            }

            for (const auto &enemy: entities) {
                DrawRectangle(enemy.get_position().x, enemy.get_position().y, enemy.get_size().x,
                              enemy.get_size().y, RED);
                DrawText(std::format("Health: {}", enemy.get_health()).c_str(),
                         enemy.get_position().x - 20, enemy.get_position().y - 20, 20, RED);
            }

            for (const auto &wall: map.get_walls()) {
                DrawRectanglePro(wall.bound, {0,0}, 0, BLUE);
            }

            EndMode2D();
        }

        else if (state == GameState::PLAYER_DEAD) {
            DrawText("you losar", 350, 280, 80, DARKGRAY);
        }

        EndDrawing();
    }
}



bool Game::load_entities_from_file(std::string file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    entities.clear();
    std::string line;

    int numRun = 0;

    while (std::getline(file, line)) {
        // Skip empty or comment lines
        if (line.empty() || line.find("//") == 0)
            continue;



        // Split at @@ if it exists
        size_t pos = line.find("@@");
        std::string numberPart = line.substr(0, pos);
        std::string entityName = (pos != std::string::npos) ? line.substr(pos + 2) : "";

        // Parse numbers
        std::istringstream iss(numberPart);
        std::string token;
        std::vector<float> values;

        while (std::getline(iss, token, ',')) {
            try {
                values.push_back(std::stof(token));
            } catch (const std::exception&) {
                std::cerr << "Invalid number in line: " << line << std::endl;
                return false;
            }
        }

        if (values.size() != 3) {
            std::cerr << "Expected 3 numbers in line: " << line << std::endl;
            return false;
        }

        if (numRun == 0)
            player = Entity(std::make_unique<PlayerBehavior>(), {values[0]*50, values[1]*50}, entityName, DEFAULT_ENTITY_SIZE_PX, values[2]);
        else
            entities.emplace_back(Entity(std::make_unique<EnemyBehavior>(), {values[0]*50, values[1]*50}, entityName, DEFAULT_ENTITY_SIZE_PX, values[2]));

        numRun++;
    }

    return true;
}