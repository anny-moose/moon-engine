#include "Entity.h"
#include "raymath.h"
#include "../Physics/Collision.h"

// AI goes here
void EnemyBehavior::move(Entity &self, GameMap &map) {
    if (player_visible) {
        self.shoot_bullet(player_center, false);
    }
}


bool EnemyBehavior::tick(Entity &self, GameMap &map) {

    player_center = player->get_position() + player->get_size()/2;

// Maybe implement a private member `bool sees_player` and move shooting into method `move()` ?
bool EnemyBehavior::tick(Entity &self, GameMap &map, const Entity &player) {
    bool can_shoot = true;
    if (Vector2Distance(self.get_position(), player.get_position()) < 1000) {
        for (const auto &wall: map.get_walls())
            if (wall.CheckCollisionLine(player.get_position(), self.get_position()))
                can_shoot = false;
        if (can_shoot)
            self.shoot_bullet(player.get_position(), false);
    }

    return true;
}
