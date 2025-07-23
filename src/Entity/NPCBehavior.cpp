#include "Entity.h"
#include "../Game/Game.h"

bool NPCBehavior::tick(Entity &self, GameMap &map) {
    if (Vector2Distance(self.get_position() / 2, player->get_position() / 2) <= 100 && IsKeyPressed(KEY_E)) {
        self.get_game().set_dialogue(this);
        talking = true;
    }

    if (talking) {
        timer -= GetFrameTime();
        self.get_game().focus_entity(current_dialogue->first);
        if (IsKeyPressed(KEY_E)) {
            if (timer <= 0.0f) {
                ++current_dialogue;
                timer = 0.1f;
            }
        }
    }

    if (current_dialogue == dialogue_text.end()) {
        current_dialogue = dialogue_text.begin();
        return talking = false;
    }

    return true;
}


void NPCBehavior::draw_dialogue() {
    DrawText(current_dialogue->second.c_str(), 0, 0, 30, DARKGRAY);
}
