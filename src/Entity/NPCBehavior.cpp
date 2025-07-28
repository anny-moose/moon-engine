#include "Entity.h"
#include "../Game/Game.h"


bool NPCBehavior::tick(Entity &self, GameMap &map) {
    if (Vector2Distance(self.get_position() / 2, player->get_position() / 2) <= 100 && IsKeyPressed(KEY_E)) {
        self.get_game().set_dialogue(this);
        talking = true;
        box = DialogueBox(Rectangle{10, 10, 500, 200}, current_dialogue->second);
    }

    if (talking) {
        timer -= GetFrameTime();
        self.get_game().focus_entity(current_dialogue->first);
        if (IsKeyPressed(KEY_E)) {
            if (timer <= 0.0f) {
                ++current_dialogue;
                if (current_dialogue != dialogue_text.end())
                    box = DialogueBox(Rectangle{10,10, 500, 200}, current_dialogue->second);
                timer = 0.1f;
            }
        }
    }


    if (current_dialogue == dialogue_text.end()) {
        current_dialogue = dialogue_text.begin();
        return talking = false;
    }

    box.set_bounds({20, 20, static_cast<float>(GetScreenWidth())-40, (static_cast<float>(GetScreenHeight())-40)/3});
    box.update_element();

    return true;
}


void NPCBehavior::draw_dialogue() {
    box.draw_element();
}
