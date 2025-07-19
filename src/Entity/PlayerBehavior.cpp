#include "Entity.h"
#include "../Game/Game.h"

Game* Entity::game = nullptr;

void PlayerBehavior::move(Entity &self, GameMap &map) {
    float delta = GetFrameTime();

    if (!self.get_frozen_state()) {
        if (IsKeyDown(KEY_W)) {
            self.add_to_velocity({0,-(self.get_speed() * delta)});
        }
        if (IsKeyDown(KEY_A)) {
            self.add_to_velocity({-(self.get_speed() * delta), 0});
        }
        if (IsKeyDown(KEY_S)) {
            self.add_to_velocity({0, (self.get_speed()*delta)});
        }
        if (IsKeyDown(KEY_D)) {
            self.add_to_velocity({(self.get_speed()*delta), 0});
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        self.shoot_bullet(GetScreenToWorld2D(GetMousePosition(), *camera), true);
    }
}

bool PlayerBehavior::tick(Entity &self, GameMap &map) {
    for (auto colIt = map.get_triggers().begin(); colIt != map.get_triggers().end();) {
        bool proceed = true;
        if (CheckCollisionRecs(self.get_hitbox(), colIt->bound)) {
            if (colIt->action == Action::FREEZE_PLAYER) {
                self.set_frozen_state(!self.get_frozen_state());
                colIt = map.get_triggers().erase(colIt);
                proceed = false;
            }
            else if (colIt->action == Action::FOCUS_CAMERA) {
                Entity::game->focus_entity(colIt->action_opt);
            }
                               }
        if (proceed) {
            ++colIt;
        }
    }

    return true;
}
