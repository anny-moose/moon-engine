#include "UIElement.h"

Game *UIElement::game = nullptr;

void Button::update_element() {
    hovered = CheckCollisionPointRec(GetMousePosition(), bounds);

    if (hovered) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            click();
        }
    }
}

inline bool cmp_vec2(Vector2 v1, Vector2 v2) {
    return (v1.x >= v2.x) && (v1.y >= v2.y);
}

void Button::draw_element() {
    if (hovered)
        DrawRectangleRoundedLinesEx(bounds, 0.5f, 16, 10.0f, RED);
    DrawRectangleRounded(bounds, 0.5f, 16, WHITE);

    float font_size = 15;


    while (cmp_vec2({bounds.width/2, bounds.height/2}, MeasureTextEx(GetFontDefault(), text.c_str(), font_size, 1))) {
        font_size += 1;
    }

    Vector2 text_size = MeasureTextEx(GetFontDefault(), text.c_str(), font_size, 1.0f);
    Vector2 position = {
        bounds.x + (bounds.width/2 - text_size.x/2),
        bounds.y + (bounds.height/2 - text_size.y/2)
    };

    DrawTextEx(GetFontDefault(), text.c_str(), position, font_size, 1, DARKGRAY);
}

void ButtonContainer::update_element() {
    if (direction == HORIZONTAL) {
        float x_current = bounds.x;
        float width = bounds.width - 20*(buttons.size()-1);
        for (auto &button: buttons) {
            button.set_bounds({x_current, bounds.y, width/buttons.size(), bounds.height});
            x_current += width/buttons.size() + 20;
        }
    }

    for (auto &button: buttons) {
        button.update_element();
    }
}

void ButtonContainer::draw_element() {
    for (auto &button: buttons) {
        button.draw_element();
    }
}