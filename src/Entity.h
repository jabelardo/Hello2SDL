//
// Created by Jose Gutierrez on 10/23/15.
//

#ifndef HELLO2SDL_ENTITY_H
#define HELLO2SDL_ENTITY_H

#include "V2D.h"
#include "Bitmap.h"
#include "Game.h"

struct GameMemory;
struct UserInput;
struct SDL_Renderer;

enum EntityType {
  NULL_ENTITY_TYPE = 0,
  PLAYER_BULLET_TYPE,
  ENEMY_BULLET_TYPE,
  PLAYER_TYPE,
  GLIDER_TYPE,
  SHOT_GLIDER_TYPE,
  ESKELETOR_TYPE,
  TURRET_TYPE,
  ROOF_TURRET_TYPE,
  LEVEL_1_BOSS_TYPE,
};

struct Entity {
  EntityType type;
  V2D position;
  Bitmap bitmap;
  V2D velocity;
  int dyingCounter;
  int dyingTime;
  int invulnerableCounter;
  int invulnerableTime;
  int bulletCounter;
  int bulletTime;
  bool decreasingAlpha;
  float maxSpeed;
  float deltaMovement;
  V2D initialPosition;
  int health;
  float halfCollisionDim;
};

struct EntityNode {
  Entity entity;
  EntityNode *next;
};

EntityType parseEntityType(const char *str);

void initEntity(Entity *entity);

void drawEntity(Entity *entity, GameContext* gameContext, SDL_Renderer *renderer);

bool checkEntityCollision(Entity *entity1, Entity *entity2);

V2D getEntityScreenPosition(Entity *entity, V2D cameraPosition);

#endif //HELLO2SDL_ENTITY_H
