#pragma once
#include "raylib.h"
#include <fstream>
#include <vector>

typedef struct {
  Vector2 position;
  Vector2 size;
  int health;
  std::string name;
  float vel_x;
  float vel_y;
  float reload_clock;
  float reload_time;
  bool frozen_state = false;
} Entity;

typedef enum {
  FREEZE_PLAYER=0,
  TRIGGER_DIALOGUE,
  FOCUS_CAMERA
} Action;

typedef struct {
  std::pair<Vector2, Vector2> area;
  Action action;
  std::string action_opt;
} TriggerArea;

typedef struct {
  std::vector<std::pair<Vector2, Vector2>> walls; // pos, size
  std::vector<TriggerArea> trigger_areas;
  Entity player;
  std::vector<Entity> enemies;
  Entity* focused_entity; // this is messy but should be rewritten whenever i decide to ditch everything and pake it OO
} GameMap;

typedef enum {
  MENU=0,
  RUNNING,
  DIALOGUE,
  DEAD,
} GameState;

// TODO: single file map format

void load_map_from_file(GameMap &map, std::string file_path);
void load_trigger_areas(GameMap &map, std::string file_path);
void load_entities_from_file(GameMap &map, std::string file_path);
