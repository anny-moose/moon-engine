#include "Collision.h"
#include "raylib.h"
#include <utility>
#include <vector>


void collision_x(const Rectangle& hitbox, float &vel_x, const std::pair<float, float> wall_x) {
    float wall_left = wall_x.first;
    float wall_right = wall_left + wall_x.second;
    float player_left = hitbox.x + vel_x;
    float player_right = player_left + hitbox.width;

    if (player_right > wall_left && player_left < wall_right) {
        if (vel_x > 0) {
            vel_x =
                    wall_left -
                    (static_cast<double>(hitbox.x) +
                     static_cast<double>(
                         hitbox.width)); // I legitimately have zero idea why, but this
            // cast allows the player to collide with walls
            // on the side normally instead of SOMETIMES
            // hovering one pixel away from them...
        } else if (vel_x < 0) {
            vel_x = wall_right - hitbox.x;
        }
    }
}

void collision_y(const Rectangle& hitbox, float &vel_y, std::pair<float, float> wall_y) {
    float wall_top = wall_y.first;
    float wall_bottom = wall_top + wall_y.second;
    float player_top = hitbox.y + vel_y;
    float player_bottom = player_top + hitbox.height;

    if (player_bottom > wall_top && player_top < wall_bottom) {
        if (vel_y > 0) {
            vel_y =
                    wall_top -
                    (static_cast<double>(hitbox.y) +
                     static_cast<double>(hitbox.height)); // Same as above. (My best guess
            // is FP precision loss?)
        } else if (vel_y < 0) {
            vel_y = wall_bottom - hitbox.y;
        }
    }
}
