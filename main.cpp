#include "Map.h"
#include "auxillary_functions.h"
#include "raylib.h"
#include "raymath.h"
#include <format>
#include <vector>

typedef struct {
    Vector2 position;
    Vector2 origin_pos;
    Vector2 end_pos;
    float speed;
    Vector2 direction = {0, 0};
    bool is_friendly;
} Bullet;

void move_player(Entity &player) {
    float delta = GetFrameTime();
    float speed = 300.0f;

    if (!player.frozen_state) {
        if (IsKeyDown(KEY_W)) {
            player.vel_y -= speed * delta;
        }
        if (IsKeyDown(KEY_A)) {
            player.vel_x -= speed * delta;
        }
        if (IsKeyDown(KEY_S)) {
            player.vel_y += speed * delta;
        }
        if (IsKeyDown(KEY_D)) {
            player.vel_x += speed * delta;
        }
    }
}

void collision_x(Entity &player, std::pair<float, float> wall_x) {
    float wall_left = wall_x.first * 50;
    float wall_right = wall_left + wall_x.second * 50;
    float player_left = player.position.x + player.vel_x;
    float player_right = player_left + player.size.x;

    if (player_right > wall_left && player_left < wall_right) {
        if (player.vel_x > 0) {
            player.vel_x =
                    wall_left -
                    (static_cast<double>(player.position.x) +
                     static_cast<double>(
                         player.size.x)); // I legitimately have zero idea why, but this
            // cast allows the player to collide with walls
            // on the side normally instead of SOMETIMES
            // hovering one pixel away from them...
        } else if (player.vel_x < 0) {
            player.vel_x = wall_right - player.position.x;
        }
    }
}

void collision_y(Entity &player, std::pair<float, float> wall_y) {
    float wall_top = wall_y.first * 50;
    float wall_bottom = wall_top + wall_y.second * 50;
    float player_top = player.position.y + player.vel_y;
    float player_bottom = player_top + player.size.y;

    if (player_bottom > wall_top && player_top < wall_bottom) {
        if (player.vel_y > 0) {
            player.vel_y =
                    wall_top -
                    (static_cast<double>(player.position.y) +
                     static_cast<double>(player.size.y)); // Same as above. (My best guess
            // is FP precision loss?)
        } else if (player.vel_y < 0) {
            player.vel_y = wall_bottom - player.position.y;
        }
    }
}

void player_tick(Entity &player, GameMap &map, GameState &state) {
    for (const auto &wall: map.walls) {
        if (CheckCollisionRecs({
                                   wall.first.x * 50, wall.first.y * 50,
                                   wall.second.x * 50, wall.second.y * 50
                               },
                               {
                                   player.position.x + player.vel_x, player.position.y,
                                   player.size.x, player.size.y
                               })) {
            collision_x(player, {wall.first.x, wall.second.x});
        } else if (CheckCollisionRecs({
                                          wall.first.x * 50, wall.first.y * 50,
                                          wall.second.x * 50, wall.second.y * 50
                                      },
                                      {
                                          player.position.x,
                                          player.position.y + player.vel_y,
                                          player.size.x, player.size.y
                                      })) {
            collision_y(player, {wall.first.y, wall.second.y});
        }
    }
    player.position += {player.vel_x, player.vel_y};
    player.vel_x = 0;
    player.vel_y = 0;

    for (auto colIt = map.trigger_areas.begin(); colIt != map.trigger_areas.end();) {
        bool proceed = true;
        if (CheckCollisionRecs({player.position.x, player.position.y, player.size.x, player.size.y},
                               {
                                   colIt->area.first.x * 50, colIt->area.first.y * 50, colIt->area.second.x * 50,
                                   colIt->area.second.y * 50
                               })) {
            if (colIt->action == Action::FREEZE_PLAYER) {
                freeze_player_toggle(map.player);
                colIt = map.trigger_areas.erase(colIt);
                proceed = false;
            }
            else if (colIt->action == Action::FOCUS_CAMERA) {
                focus_camera_on_entity(map, colIt->action_opt);
            }
        }
        if (proceed) {
            ++colIt;
        }
    }


    if (player.health <= 0) {
        state = GameState::DEAD;
    }

    player.reload_clock -= GetFrameTime();
}

void shoot_bullet(Entity &player, std::vector<Bullet> &bullets_arr,
                  Camera2D &camera) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if (player.reload_clock <= 0) {
            Vector2 origin_pos = player.position + player.size / 2;
            bullets_arr.push_back((Bullet){
                .position = origin_pos,
                .origin_pos = origin_pos,
                .end_pos = GetScreenToWorld2D(GetMousePosition(), camera),
                .speed = 500.0f,
                .is_friendly = true,
            });
            player.reload_clock = player.reload_time;
        }
    }
}

void move_bullet(Bullet &bullet) {
    float delta = GetFrameTime();

    if (bullet.direction ==
        (Vector2){0, 0}) {
        // Bit of a hack but when will it ever matter?
        Vector2 dir_v = Vector2Subtract(bullet.end_pos, bullet.origin_pos);
        bullet.direction = Vector2Normalize(dir_v);
    }

    bullet.position += bullet.direction * bullet.speed * delta;
}

void bullets_tick(std::vector<Bullet> &bullets,
                  std::vector<std::pair<Vector2, Vector2> > walls,
                  std::vector<Entity> &enemies, Entity &player) {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bool hit = false;

        move_bullet(*bulletIt);

        for (auto &wall: walls) {
            if (CheckCollisionRecs(
                {bulletIt->position.x, bulletIt->position.y, 24, 24},
                {
                    wall.first.x * 50, wall.first.y * 50, wall.second.x * 50,
                    wall.second.y * 50
                })) {
                hit = true;
                bulletIt = bullets.erase(bulletIt);
                break;
            }
        }

        if (bulletIt->is_friendly) {
            for (auto &enemy: enemies) {
                if (CheckCollisionRecs(
                    {bulletIt->position.x, bulletIt->position.y, 24, 24},
                    {
                        enemy.position.x, enemy.position.y, enemy.size.x,
                        enemy.size.y
                    })) {
                    hit = true;
                    enemy.health -= 10;
                    bulletIt = bullets.erase(bulletIt);
                    break;
                }
            }
        } else {
            if (CheckCollisionRecs(
                {bulletIt->position.x, bulletIt->position.y, 24, 24},
                {
                    player.position.x, player.position.y, player.size.x,
                    player.size.y
                })) {
                player.health -= 10;
                hit = true;
                bulletIt = bullets.erase(bulletIt);
            }
        }

        // give bullets a lifetime
        if (Vector2Distance(bulletIt->origin_pos, bulletIt->position) > 1000) {
            hit = true;
            bulletIt = bullets.erase(bulletIt);
        }

        if (!hit) {
            ++bulletIt;
        }
    }
}

bool CheckCollisionLineWall(std::pair<Vector2, Vector2> line1,
                            std::pair<Vector2, Vector2> wall) {
    std::vector<std::pair<Vector2, Vector2> > wall_lines = {
        {
            {wall.first.x * 50, wall.first.y * 50},
            {
                wall.first.x * 50 * wall.second.x * 50,
                wall.first.y * 50 * wall.second.y * 50
            }
        },
        {
            {
                wall.first.x * 50 * wall.second.x * 50,
                wall.first.y * 50 * wall.second.y * 50
            },
            {wall.first.x * 50, wall.second.y * 50}
        }
    };

    for (auto &line: wall_lines) {
        if (CheckCollisionLines(line.first, line.second, line1.first, line1.second,
                                nullptr)) {
            return true;
        }
    }
    return false;
}

void enemies_tick(GameMap &map, std::vector<Bullet> &bullets) {
    for (auto enemyIt = map.enemies.begin(); enemyIt != map.enemies.end();) {
        if (enemyIt->health <= 0) {
            enemyIt = map.enemies.erase(enemyIt);
        } else {
            bool can_shoot = true;
            if (Vector2Distance(enemyIt->position, map.player.position) < 1000) {
                for (auto &wall: map.walls)
                    if (CheckCollisionLineWall({map.player.position, enemyIt->position},
                                               wall))
                        can_shoot = false;
                if (can_shoot)
                    if (enemyIt->reload_clock <= 0) {
                        bullets.push_back((Bullet){
                            .position = enemyIt->position,
                            .origin_pos = enemyIt->position,
                            .end_pos = map.player.position,
                            .speed = 500.0f,
                            .is_friendly = false,
                        });
                        enemyIt->reload_clock = enemyIt->reload_time;
                    }
            }
            enemyIt->reload_clock -= GetFrameTime();
            ++enemyIt;
        }
    }
}

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib C++ App");
    SetTargetFPS(60);

    GameMap map;

    load_map_from_file(map, "map.txt");
    load_trigger_areas(map, "areas.txt");
    load_entities_from_file(map, "entities.txt");

    GameState state = GameState::RUNNING;

    map.focused_entity = &map.player;

    Camera2D camera = {0};
    camera.target = {
        map.focused_entity->position.x + map.focused_entity->size.x / 2,
        map.focused_entity->position.y + map.focused_entity->size.y / 2
    };
    camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
    camera.zoom = 1.0f;

    std::vector<Bullet> bullets;

    while (!WindowShouldClose()) {
        // Events
        move_player(map.player);
        shoot_bullet(map.player, bullets, camera);

        if (IsKeyPressed(KEY_P)) {
            freeze_player_toggle(map.player);
            focus_camera_on_entity(map, "enemy1");
        }

        // Logic

        if (state == GameState::RUNNING) {
            player_tick(map.player, map, state);
            bullets_tick(bullets, map.walls, map.enemies, map.player);
            enemies_tick(map, bullets);
        }

        const float smoothness = 0.1f;
        camera.target +=
        {
            ((map.focused_entity->position.x + map.focused_entity->size.x / 2) -
             camera.target.x) *
            smoothness,
            ((map.focused_entity->position.y + map.focused_entity->size.y / 2) -
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

            DrawRectangle(map.player.position.x, map.player.position.y,
                          map.player.size.x, map.player.size.y, DARKGREEN);
            DrawText(std::format("Health: {}", map.player.health).c_str(),
                     map.player.position.x - 20, map.player.position.y - 20, 20, GREEN);

            for (const auto &bullet: bullets) {
                DrawRectangle(bullet.position.x, bullet.position.y, 12, 12, WHITE);
            }

            for (const auto &enemy: map.enemies) {
                DrawRectangle(enemy.position.x, enemy.position.y, enemy.size.x,
                              enemy.size.y, RED);
                DrawText(std::format("Health: {}", enemy.health).c_str(),
                         enemy.position.x - 20, enemy.position.y - 20, 20, RED);
            }

            for (const auto &wall: map.walls) {
                DrawRectangle(wall.first.x * 50, wall.first.y * 50, wall.second.x * 50,
                              wall.second.y * 50, BLUE);
            }

            EndMode2D();
        }

        else if (state == DEAD) {
            DrawText("you losar", 350, 280, 80, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
