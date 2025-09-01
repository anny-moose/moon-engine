#pragma once
#include <map>
#include <memory>
#include "raylib.h"
#include "raymath.h"
#include "../Game/Map.h"
#include "../Game/UI/UIElement.h"
#include <string>
#include <utility>

class DialogueBox;
class Game;
class BulletManager;
class Entity;


constexpr Vector2 DEFAULT_ENTITY_SIZE_PX = {50, 50};
constexpr float DEFAULT_RELOAD_TIME_SECOND = 0.50f;
constexpr int DEFAULT_HEALTH_VALUE = 100;
constexpr float DEFAULT_SPEED_VALUE = 300.0f;

class EntityBehavior {
public:
    static const Entity* player;

    virtual void move(Entity &self, GameMap &map) = 0;
    virtual bool tick(Entity &self, GameMap &map) = 0; // false if mark for deletion
    virtual void draw(const Entity &self) = 0;
    virtual ~EntityBehavior() = default;
};

class Entity {
private:
    std::string id;
    Rectangle hitbox;

    int health_max;
    int health;

    float speed;
    float vel_x;
    float vel_y;

    float reload_clock;
    float reload_time;

    bool is_frozen = false;

    float invulnerability_time = 0.0f;

    std::unique_ptr<EntityBehavior> behavior;
public:
    explicit Entity(std::unique_ptr<EntityBehavior> behavior, Vector2 position, std::string id = "", Vector2 size = DEFAULT_ENTITY_SIZE_PX, int health = DEFAULT_HEALTH_VALUE, float reload_time = DEFAULT_RELOAD_TIME_SECOND, float speed = DEFAULT_SPEED_VALUE)
    : id(std::move(id)), health_max(health), health(health), reload_time(reload_time), behavior(std::move(behavior)), speed(speed) {
        hitbox.x = position.x;
        hitbox.y = position.y;
        hitbox.width = size.x;
        hitbox.height = size.y;
        reload_clock = 0.0f;
        is_frozen = false;
        vel_x = 0;
        vel_y = 0;
    }

    // doesn't work if no behavior/id set, id-based comparison is not very reliable as id's are non-unique //todo: make id's unique or add static counter for entities and compare based on that
    bool operator==(const Entity& npc) const {
        if (npc.behavior == behavior && behavior != nullptr)
            return true;
        if (npc.id == id && !id.empty())
            return true;
        return false;
    }


    const std::string& get_id() const { return id; }

    Vector2 get_position() const { return (Vector2){hitbox.x, hitbox.y }; }
    Vector2 get_size() const { return (Vector2){hitbox.width, hitbox.height}; }
    Rectangle get_hitbox() const { return hitbox; }

    void set_health(int new_health) { health = new_health; }
    int get_health() const { return health; }
    int get_max_health() const { return health_max; }

    static BulletManager* manager;

    static Game* game;
    Game& get_game() { return *game; }

    float get_speed() const { return speed; }
    void add_to_velocity(Vector2 increment);

    void reset_velocity() {
        vel_x = 0;
        vel_y = 0;
    }

    bool can_shoot() const { return reload_clock <= 0; }
    void reset_reload_clock() { reload_clock = reload_time; }

    float get_reload_clock() const { return reload_clock; }

    bool get_frozen_state() const { return is_frozen; }
    void set_frozen_state(bool new_state) { is_frozen = new_state; }

    void set_invulnerability_time(float new_time) { invulnerability_time = new_time; }
    float get_invulnerability_time() const { return invulnerability_time; }

    void move(GameMap &map) { if (behavior != nullptr) behavior->move(*this, map); }
    void shoot_bullet(Vector2 end_pos, bool is_friendly = true);
    bool tick(GameMap &map);

    void draw() const { behavior->draw(*this); }

    void set_behavior(std::unique_ptr<EntityBehavior> new_behavior) { behavior = std::move(new_behavior); }
};


class PlayerBehavior final : public EntityBehavior {
public:
    static Camera2D* camera;

    void move(Entity &self, GameMap &map) override;
    void draw(const Entity &self) override;
    bool tick(Entity &self, GameMap &map) override;
    ~PlayerBehavior() = default;
};


typedef enum {
    NORMAL = 0,
    WARDEN,
} EnemyType;

class EnemyBehavior final : public EntityBehavior {
private:
    EnemyType type;

    float line_of_sight_length = 500;
    bool player_visible = false;
    Vector2 player_center;

    float locked_on_timer = 0.0f;
    Vector2 movement_direction = Vector2Zero();
public:
    explicit EnemyBehavior(EnemyType type = EnemyType::NORMAL) : type(type) {}

//    static const Entity* player;

    void move(Entity &self, GameMap &map) override;
    void draw(const Entity &self) override;
    bool tick(Entity &self, GameMap &map) override;
    ~EnemyBehavior() = default;
};


class NPCBehavior final : public EntityBehavior {
    std::vector<std::pair<std::string, std::string>> dialogue_text;
    std::vector<std::pair<std::string, std::string>>::const_iterator current_dialogue = dialogue_text.begin();
    bool talking = false;
    float timer = 0.1f;
    DialogueBox box = DialogueBox({0,0,0,0});
public:
    explicit NPCBehavior(std::vector<std::pair<std::string, std::string>> dialogue_text) : dialogue_text(std::move(dialogue_text)) {}

    void move(Entity &self, GameMap &map) override {};
    void draw(const Entity &self) override;
    bool tick(Entity &self, GameMap &map) override;
    void draw_dialogue();
};