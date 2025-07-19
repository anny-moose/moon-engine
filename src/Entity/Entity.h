#pragma once
#include <memory>
#include "raylib.h"
#include "../Game/Map.h"
#include <string>

class Game;
class BulletManager;
class Entity;


constexpr Vector2 DEFAULT_ENTITY_SIZE_PX = {50, 50};
constexpr float DEFAULT_RELOAD_TIME_SECOND = 0.50f;
constexpr int DEFAULT_HEALTH_VALUE = 100;

constexpr Vector2 DEFAULT_BULLED_SIZE_PX = {24, 24};
constexpr float DEFAULT_BULLET_SPEED = 500.0f;


class EntityBehavior {
public:
    virtual void move(Entity &self, GameMap &map) = 0;
    virtual bool tick(Entity &self, GameMap &map) = 0; // false if mark for deletion
    virtual ~EntityBehavior() = default;
};

class Entity {
private:
    std::string id;
    Rectangle hitbox;
    int health;

    float vel_x;
    float vel_y;

    float reload_clock;
    float reload_time;

    bool is_frozen = false;

    std::unique_ptr<EntityBehavior> behavior; //replace with unique_ptr or just EntityBehavior if/when behaviors have their own states


public:
    explicit Entity(std::unique_ptr<EntityBehavior> behavior, Vector2 position, std::string id = "", Vector2 size = DEFAULT_ENTITY_SIZE_PX, int health = DEFAULT_HEALTH_VALUE, float reload_time = DEFAULT_RELOAD_TIME_SECOND)
    : id(id), health(health), reload_time(reload_time), behavior(std::move(behavior)) {
        hitbox.x = position.x;
        hitbox.y = position.y;
        hitbox.width = size.x;
        hitbox.height = size.y;
        reload_clock = 0.0f;
        is_frozen = false;
        vel_x = 0;
        vel_y = 0;
    }

    const std::string& get_id() const { return id; }

    Vector2 get_position() const { return (Vector2){hitbox.x, hitbox.y }; }
    Vector2 get_size() const { return (Vector2){hitbox.width, hitbox.height}; }

    Rectangle get_hitbox() const { return hitbox; }

    void set_health(int new_health) { health = new_health; }
    int get_health() const { return health; }

    void reset_velocity() {
        vel_x = 0;
        vel_y = 0;
    }

    static BulletManager* manager;
    static Game* game;

    void add_to_velocity(Vector2 increment);

    bool can_shoot() const { return reload_clock <= 0; }
    void reset_reload_clock() { reload_clock = reload_time; }

    bool get_frozen_state() const { return is_frozen; }
    void set_frozen_state(bool new_state) { is_frozen = new_state; }

    void move(GameMap &map) { if (behavior != nullptr) behavior->move(*this, map); }
    void shoot_bullet(Vector2 end_pos, bool is_friendly = true);
    bool tick(GameMap &map);
};


class PlayerBehavior final : public EntityBehavior {
public:
    static Camera2D* camera;

    void move(Entity &self, GameMap &map) override;

    bool tick(Entity &self, GameMap &map) override;
    ~PlayerBehavior() = default;
};


class EnemyBehavior final : public EntityBehavior {
private:
    float line_of_sight_length = 1000;
    bool player_visible = false;
    Vector2 player_center;
public:
    static const Entity* player;

    void move(Entity &self, GameMap &map) override;
    bool tick(Entity &self, GameMap &map) override;
    ~EnemyBehavior() = default;
};
