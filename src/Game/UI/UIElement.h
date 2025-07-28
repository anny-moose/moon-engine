#pragma once
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "raylib.h"

constexpr float TEXT_SPEED_DEFAULT = 0.05f;

class Game;

class UIElement {
protected:
    Rectangle bounds = {};
public:
    explicit UIElement(Rectangle bounds) : bounds(bounds) {}
    virtual ~UIElement() = default;
    static Game* game;
    virtual void update_element() = 0;
    virtual void draw_element() = 0;
    virtual void set_bounds(Rectangle new_bounds) = 0;
};

class Button : public UIElement {
private:
    std::function<void(Game& game)> on_click;
    std::string text;

    float font_size = 10;
    Vector2 text_pos;

    Vector2 screen_resolution;

    bool hovered = false;
public:
    explicit Button(Rectangle bounds, std::function<void(Game& game)> on_click, std::string text = "") : UIElement(bounds),
            on_click(std::move(on_click)), text(std::move(text)) {}

    std::string get_text() const { return text; }

    void set_bounds(Rectangle new_bounds) override { bounds = new_bounds; }

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

class Container : public UIElement {
private:
    std::vector<std::unique_ptr<UIElement>> elements;

    Container_Direction direction;
public:
    explicit Container(Rectangle bounds, Container_Direction dir = HORIZONTAL) : UIElement(bounds), direction(dir) {}

    template <typename T, typename... Args>
    void emplace_back(Args&&... args) {
        elements.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void push_back(std::unique_ptr<UIElement> element) {
        elements.push_back(std::move(element));
    }

    void set_bounds(Rectangle new_bounds) override { bounds = new_bounds; }

    void update_element() override;
    void draw_element() override;

};

class DialogueBox : public UIElement {
private:
    std::string text;
    std::string displayed_text;
    int text_idx = 0;

    float text_speed = TEXT_SPEED_DEFAULT;

    float font_size = 10;
    Vector2 text_pos;
    Vector2 screen_resolution = {0,0};


public:
    explicit DialogueBox(Rectangle bounds, std::string text = "") : UIElement(bounds), text(std::move(text)) {}

    void update_element() override;
    void draw_element() override;

    //                                                                    v           hack as fuck          v
    void set_bounds(Rectangle new_bounds) override { bounds = new_bounds; screen_resolution = {0,0}; }
};