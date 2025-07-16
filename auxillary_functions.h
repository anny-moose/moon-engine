#include "Map.h"
#include <raylib.h>

inline void freeze_player_toggle(Entity &player) {
  player.frozen_state = !player.frozen_state;
}

inline void focus_camera_on_entity(GameMap& map, std::string entity_id) {
  for (auto& entity : map.enemies) {
    if (entity_id == entity.name) {
      map.focused_entity = &entity;
    }
  }
  if (entity_id == map.player.name)
    map.focused_entity = &map.player;
}


