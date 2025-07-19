#include "Entity.h"
#include "../Physics/Collision.h"
#include "Bullet.h"

BulletManager* Entity::manager = nullptr;
Camera2D* PlayerBehavior::camera = nullptr;

void Entity::shoot_bullet(Vector2 end_pos, bool is_friendly) {
    if (reload_clock <= 0) {
        Vector2 origin_pos = {(hitbox.x + hitbox.width / 2),
            (hitbox.y + hitbox.height / 2)};

        manager->append(Bullet(origin_pos, end_pos, {vel_x, vel_y}, is_friendly));

        reload_clock = reload_time;
    }
}


void Entity::add_to_velocity(Vector2 increment) {
    vel_x = vel_x + increment.x <= 500 ? vel_x + increment.x : 500;
    vel_y = vel_x + increment.y <= 500 ? vel_y + increment.y : 500;
}


bool Entity::tick(GameMap &map) {
    for (const auto &wall: map.get_walls()) {
        if (CheckCollisionRecs(wall.bound, {hitbox.x + vel_x, hitbox.y, hitbox.width, hitbox.height})) {
            collision_x(hitbox, vel_x, {wall.bound.x, wall.bound.width});
        } else if (CheckCollisionRecs(wall.bound, {hitbox.x, hitbox.y + vel_y, hitbox.width, hitbox.height})) {
            collision_y(hitbox, vel_y, {wall.bound.y, wall.bound.height});
        }
    }
    hitbox.x += vel_x;
    hitbox.y += vel_y;
    reset_velocity();

    if (behavior != nullptr) {
        if (!behavior->tick(*this, map)) return false;
    }

    reload_clock -= GetFrameTime();

    if (health <= 0) return false;

    return true;
}



