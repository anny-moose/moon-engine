#include "UIElement.h"

Game *UIElement::game = nullptr;


inline bool cmp_vec2(Vector2 v1, Vector2 v2) {
    return (v1.x >= v2.x) && (v1.y >= v2.y);
}

void Button::update_element() {
    hovered = CheckCollisionPointRec(GetMousePosition(), bounds);

    if (hovered) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            click();
        }
    }

    if (screen_resolution.x != GetScreenWidth() || screen_resolution.y != GetScreenHeight()) {
        font_size = 10;
        while (cmp_vec2({bounds.width/2, bounds.height/2}, MeasureTextEx(GetFontDefault(), text.c_str(), font_size, 1))) {
            font_size += 1;
        }


        Vector2 text_size = MeasureTextEx(GetFontDefault(), text.c_str(), font_size, 1.0f);
        text_pos = {
            bounds.x + (bounds.width/2 - text_size.x/2),
            bounds.y + (bounds.height/2 - text_size.y/2)
        };

        screen_resolution = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    }
}

void Button::draw_element() {
    if (hovered)
        DrawRectangleRoundedLinesEx(bounds, 0.5f, 16, 10.0f, RED);
    DrawRectangleRounded(bounds, 0.5f, 16, WHITE);


    DrawTextEx(GetFontDefault(), text.c_str(), text_pos, font_size, 1, DARKGRAY);
}

void Container::update_element() {
    if (direction == HORIZONTAL) {
        float x_current = bounds.x;
        float width = bounds.width - 20*(elements.size()-1);
        for (auto &element: elements) {
            element->set_bounds({x_current, bounds.y, width/elements.size(), bounds.height});
            x_current += width/elements.size() + 20;
        }
    } else {
        float y_current = bounds.y;
        float height = bounds.height - 20*(elements.size()-1);
        for (auto &element: elements) {
            element->set_bounds({bounds.x, y_current, bounds.width, height/elements.size()});
            y_current += height/elements.size() + 20;
        }
    }

    for (auto &element: elements) {
        element->update_element();
    }
}


void Container::draw_element() {
    for (auto &element: elements) {
        element->draw_element();
    }
}

void DialogueBox::update_element() {
    if (text != "")
    if (screen_resolution.x != GetScreenWidth() || screen_resolution.y != GetScreenHeight()) {
        font_size = 10;
        while (cmp_vec2({bounds.width/2, bounds.height/2}, MeasureTextEx(GetFontDefault(), text.c_str(), font_size, 2))) {
            font_size += 1;
            if (font_size == 50) break;
        }


        Vector2 text_size = MeasureTextEx(GetFontDefault(), text.c_str(), font_size, 2.0f);
        text_pos = {
            bounds.x + (bounds.width/2 - text_size.x/2),
            bounds.y + (bounds.height/2 - text_size.y/2)
        };

        screen_resolution = {static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
    }
}

void DialogueBox::draw_element() {
    DrawRectangleRoundedLinesEx(bounds, 0.5f, 16, 10.0f, WHITE);
    DrawRectangleRounded(bounds, 0.5f, 16, BLACK);

    DrawTextEx(GetFontDefault(), text.c_str(), text_pos, font_size, 2, WHITE);
}

