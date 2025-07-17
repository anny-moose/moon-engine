#include "Bullet.h"

void Bullet::move_bullet() {
    Vector2 increment = direction * speed * GetFrameTime();

    hitbox.x += increment.x;
    hitbox.y += increment.y;
}

bool Bullet::tick(const std::vector<Wall> &walls, std::vector<Entity> &enemies, Entity &player) {
    move_bullet();

    for (auto &wall: walls) {
        if (CheckCollisionRecs(hitbox, wall.bound)) return false;
    }

    if (is_friendly) {
        for (auto &enemy: enemies) {
            if (CheckCollisionRecs(hitbox, enemy.get_hitbox())) {
                enemy.set_health(enemy.get_health() - 10);
                return false;
            }
        }
    } else {
        if (CheckCollisionRecs(hitbox, player.get_hitbox())) {
            player.set_health(player.get_health() -  10);
            return false;
        }
    }

    // give bullets a lifetime
    if (Vector2Distance(origin, {hitbox.x, hitbox.y}) > 1000) return false;

    return true;
}

void BulletManager::logic_tick(const std::vector<Wall> &walls, std::vector<Entity> &enemies, Entity &player) {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bulletIt->move_bullet();
        if (!bulletIt->tick(walls, enemies, player)) {
            bullets.erase(bulletIt);
        } else ++bulletIt;
    }
}

void BulletManager::append(Bullet bullet) {
    bullets.push_back(bullet);
}