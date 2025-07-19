#include "Game.h"
#include <iostream>
#include <fstream>

void Game::focus_entity(std::string entity_id) {
    for (auto &entity: npcs) {
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
            bullet_manager.logic_tick(map.get_walls(), npcs, player);
            for (auto enemyIt = npcs.begin(); enemyIt != npcs.end();) {
                enemyIt->move(map);
                if (!enemyIt->tick(map))
                    enemyIt = npcs.erase(enemyIt);
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

            for (const auto &enemy: npcs) {
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

    if (!file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return false;
    }

    npcs.clear();

    // i really cba to make it better
    typedef struct {
        std::string name;
        Vector2 position;
        Vector2 size;
        int health;
        float reload_time;
    } entity_skeleton;

    entity_skeleton entity_temp;

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
            entity_temp.health = currentNumber;
            entity_temp.name = name;
            name.clear();
            if (firstRun) player = Entity(std::make_unique<PlayerBehavior>(), entity_temp.position, entity_temp.name, DEFAULT_ENTITY_SIZE_PX, entity_temp.health);
            else npcs.emplace_back(std::make_unique<EnemyBehavior>(), entity_temp.position, entity_temp.name, DEFAULT_ENTITY_SIZE_PX, entity_temp.health);
            entity_temp = {};
            currentNumber = 0;
            firstRun = false;
        }
    }
    return true;
}
