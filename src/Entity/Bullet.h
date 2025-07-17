#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Entity.h"
#include "../Game/Map.h"
#include <vector>
#include <memory>

constexpr float BULLET_DEFAULT_SPEED = 500.0f;
constexpr Vector2 BULLET_DEFAULT_SIZE = {24, 24};

class Bullet {
private:
    Rectangle hitbox;
    Vector2 direction = Vector2Zero();

    Vector2 origin;

    float speed;

    bool is_friendly;

public:
    explicit Bullet(Vector2 position, Vector2 destination_pos, bool is_friendly = true, float speed = BULLET_DEFAULT_SPEED,
                    Vector2 size = BULLET_DEFAULT_SIZE) : origin(position), speed(speed), is_friendly(is_friendly) {
        hitbox = {position.x, position.y, size.x, size.y};

        Vector2 dir_v = Vector2Subtract(destination_pos, position);
        direction = Vector2Normalize(dir_v);
    }

    Rectangle get_hitbox() const { return hitbox; }

    void move_bullet();

    bool tick(const std::vector<Wall> &walls, std::vector<Entity> &enemies, Entity &player);
    // ^ returns false if bullet collides
};

class BulletManager {
private:
    std::vector<Bullet> bullets;

public:
    const std::vector<Bullet>* get_bullets() const { return &bullets; }
    void logic_tick(const std::vector<Wall> &walls, std::vector<Entity> &enemies, Entity &player);
    void append(Bullet bullet);
};
