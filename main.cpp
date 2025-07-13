#include <format>
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "Map.h"

typedef struct {
    Vector2 position;
    Vector2 size;
    int health;
    float vel_x;
    float vel_y;
} Entity;


void move_player(Entity& player) {
    float delta = GetFrameTime();
    float speed = 300.0f;

    if (IsKeyDown(KEY_W)) {
        player.vel_y -= speed*delta;
    }
    if (IsKeyDown(KEY_A)) {
        player.vel_x -= speed*delta;
    }
    if (IsKeyDown(KEY_S)) {
        player.vel_y += speed*delta;
    }
    if (IsKeyDown(KEY_D)) {
        player.vel_x += speed*delta;
    }
}

void collision_x(Entity& player, std::pair<float, float> wall_x) {
    float wall_left = wall_x.first * 50;
    float wall_right = wall_left + wall_x.second * 50;
    float player_left = player.position.x + player.vel_x;
    float player_right = player_left + player.size.x;

    if (player_right > wall_left && player_left < wall_right) {
        if (player.vel_x > 0) {
            player.vel_x = wall_left - (static_cast<double>(player.position.x) + static_cast<double>(player.size.x)); // I legitimately have zero idea why, but this cast allows
                                                                                                                      // the player to collide with walls on the side normally
                                                                                                                      // instead of SOMETIMES hovering one pixel away from them...
        }
        else if (player.vel_x < 0) {
            player.vel_x = wall_right - player.position.x;
        }
    }
}

void collision_y(Entity& player, std::pair<float, float> wall_y) {
    float wall_top = wall_y.first * 50;
    float wall_bottom = wall_top + wall_y.second * 50;
    float player_top = player.position.y + player.vel_y;
    float player_bottom = player_top + player.size.y;

    if (player_bottom > wall_top && player_top < wall_bottom) {
        if (player.vel_y > 0) {
            player.vel_y = wall_top - (static_cast<double>(player.position.y) + static_cast<double>(player.size.y));  // Same as above. (My best guess is FP precision loss?)
        }
        else if (player.vel_y < 0) {
            player.vel_y = wall_bottom - player.position.y;
        }
    }
}

void player_tick(Entity& player, GameMap& map) {
    for (const auto& wall : map.walls) {
        if (CheckCollisionRecs({wall.first.x*50, wall.first.y*50, wall.second.x*50, wall.second.y*50},
                               {player.position.x+player.vel_x, player.position.y, player.size.x, player.size.y})) {
            collision_x(player, {wall.first.x, wall.second.x});
        }
        else if (CheckCollisionRecs({wall.first.x*50, wall.first.y*50, wall.second.x*50, wall.second.y*50},
                               {player.position.x, player.position.y+player.vel_y, player.size.x, player.size.y})) {
            collision_y(player, {wall.first.y, wall.second.y});
        }
    }
    player.position += {player.vel_x, player.vel_y};
    player.vel_x = 0;
    player.vel_y = 0;
}


typedef struct {
    Vector2 position;
    Vector2 origin_pos;
    Vector2 end_pos;
    float speed;
    Vector2 direction = {0, 0};
} Bullet;

void shoot_bullet(std::vector<Bullet>& bullets_arr, Vector2 origin_pos, Camera2D& camera) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bullets_arr.push_back((Bullet){
            .position = origin_pos,
            .origin_pos = origin_pos,
            .end_pos = GetScreenToWorld2D(GetMousePosition(), camera),
            .speed = 500.0f,
        });
    }
}

void move_bullet(Bullet& bullet) {
    float delta = GetFrameTime();

    if (bullet.direction == (Vector2){0, 0}) { // Bit of a hack but when will it ever matter?
        Vector2 dir_v = Vector2Subtract(bullet.end_pos, bullet.origin_pos);
        bullet.direction = Vector2Normalize(dir_v);
    }

    bullet.position += bullet.direction*bullet.speed*delta;
}

void bullets_tick(std::vector<Bullet>& bullets, std::vector<Entity>& enemies) {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
        bool hit = false;

        move_bullet(*bulletIt);
        for (auto& enemy : enemies) {
            if (CheckCollisionRecs({bulletIt->position.x, bulletIt->position.y, 24, 24}, {enemy.position.x, enemy.position.y, enemy.size.x, enemy.size.y})) {
                hit = true;
                enemy.health -= 10;
                bulletIt = bullets.erase(bulletIt);
                break;
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

void enemies_tick(std::vector<Entity>& Enemies) {
    for (auto enemyIt = Enemies.begin(); enemyIt != Enemies.end(); ) {
        if (enemyIt->health <= 0) {
            enemyIt = Enemies.erase(enemyIt);
        }
        else {
            ++enemyIt;
        }
    }
}

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib C++ App");
    SetTargetFPS(60);

    Entity player{
        .position = {100, 100},
        .size = {50, 50},
        .health = 100
    };

    std::vector<Entity> enemies = {
        {
            .position = {200, 100},
            .size = {50, 50},
            .health = 100,
        },
        {
            .position = {100, 200},
            .size = {50, 50},
            .health = 100,
        },
    };

    GameMap map = read_from_file("map.txt");

    Camera2D camera = { 0 };
    camera.target = { player.position.x + player.size.x / 2, player.position.y + player.size.y / 2 };
    camera.offset = { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f };
    camera.zoom = 1.0f;

    std::vector<Bullet> bullets;

    while (!WindowShouldClose()) {

        // Events
        move_player(player);
        shoot_bullet(bullets, Vector2Add(player.position, player.size/2), camera);

        if (IsKeyDown(KEY_P)) {
            printf("player pos: %f %f\n", player.position.x, player.position.y);
            printf("player bottom pos y: %f\n", player.position.y+player.size.y);
        }

        // Logic

        player_tick(player, map);
        bullets_tick(bullets, enemies);
        enemies_tick(enemies);

        camera.target = { player.position.x + player.size.x / 2, player.position.y + player.size.y / 2 };


        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        DrawText("Hello, Raylib!", 350, 280, 20, DARKGRAY);

        DrawRectangle(player.position.x, player.position.y, player.size.x, player.size.y, DARKGREEN);
        DrawText(std::format("Health: {}", player.health).c_str(), player.position.x - 20, player.position.y - 20, 20, GREEN);

        for (const auto& bullet : bullets) {
            DrawRectangle(bullet.position.x, bullet.position.y, 12, 12, BLACK);
        }

        for (const auto& enemy : enemies) {
            DrawRectangle(enemy.position.x, enemy.position.y, enemy.size.x, enemy.size.y, RED);
            DrawText(std::format("Health: {}", enemy.health).c_str(), enemy.position.x - 20, enemy.position.y - 20, 20, RED);
        }

        for (const auto& wall : map.walls) {
            DrawRectangle(wall.first.x*50, wall.first.y*50, wall.second.x*50, wall.second.y*50, BLUE);
        }

        EndMode2D();


        EndDrawing();
    }

    CloseWindow();
    return 0;
}