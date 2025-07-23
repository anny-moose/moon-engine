#pragma once
#include <functional>
#include <string>
#include <utility>

#include "raylib.h"

class Game;

class UIElement {
public:
    virtual ~UIElement() = default;
    static Game* game;
    virtual void update_element() = 0;
    virtual void draw_element() = 0;
};

class Button : public UIElement {
private:
    Rectangle bounds;
    std::function<void(Game& game)> on_click;
    std::string text;

    bool hovered = false;
public:
    explicit Button(Rectangle bounds, std::function<void(Game& game)> on_click, std::string text = "") : bounds(bounds),
            on_click(std::move(on_click)), text(std::move(text)) {}

    std::string get_text() const { return text; }
    void set_bounds(Rectangle new_bounds) { bounds = new_bounds; }

    void update_element() override;
    void draw_element() override;

    void click() {
        on_click(*game);
    }
};

typedef enum {
    VERTICAL = 0,
    HORIZONTAL
} Container_Direction;

class ButtonContainer : public UIElement {
private:
    Rectangle bounds;
    std::vector<Button> buttons;

    Container_Direction direction = HORIZONTAL;
public:
    explicit ButtonContainer(Rectangle bounds) : bounds(bounds) {}

    template <typename... Args>
    void emplace_back(Args&&... args) {
        buttons.emplace_back(std::forward<Args>(args)...);
    }

    void set_bounds(Rectangle new_bounds) { bounds = new_bounds; }

    void update_element() override;
    void draw_element() override;

};