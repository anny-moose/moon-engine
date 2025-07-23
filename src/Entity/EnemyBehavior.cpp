#include <mutex>

#include "Entity.h"
#include "raymath.h"
#include "../Physics/Collision.h"

const Entity *EntityBehavior::player = nullptr;

// AI goes here
void EnemyBehavior::move(Entity &self, GameMap &map) {
    if (type != EnemyType::WARDEN)
        if (locked_on_timer > 0) {
            self.add_to_velocity(movement_direction * self.get_speed() * GetFrameTime());
        }

    if (player_visible) {
        self.shoot_bullet(player_center, false);

        movement_direction = Vector2Normalize(Vector2Subtract(player->get_position(), self.get_position()));
    }
}


bool EnemyBehavior::tick(Entity &self, GameMap &map) {
    player_center = player->get_position() + player->get_size() / 2;

    bool can_shoot_player = true;
    // Assume we see player ^

    // Check whether we do v
    if (Vector2Distance(self.get_position() + self.get_size() / 2, player_center) < line_of_sight_length) {
        for (const auto &wall: map.get_walls())
            if (wall.CheckCollisionLine(player_center, self.get_position()))
                can_shoot_player = false;
    } else {
        can_shoot_player = false;
    }

    // Assign to actual variable v
    player_visible = can_shoot_player;

    if (player_visible) {
        locked_on_timer = 1.5f;
    }

    locked_on_timer -= GetFrameTime();
    return true;
}
