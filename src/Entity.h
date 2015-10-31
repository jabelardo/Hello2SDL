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
  PLAYER_TYPE,
  GLIDER_TYPE,
  SHOT_GLIDER_TYPE,
  LEVEL_1_BOSS_TYPE,
  TURRET_TYPE,
  ROOF_TURRET_TYPE,
  ESKELETOR_TYPE,
  PLAYER_BULLET_TYPE,
  ENEMY_BULLET_TYPE,
};

struct Entity {
  EntityType type;
  V2D position;
  Bitmap bitmap;
  V2D velocity;
  V2D acceleration;
  int dyingCounter;
  int dyingTime;
  int invulnerableCounter;
  int invulnerableTime;
  int bulletCounter;
  int bulletTime;
  int currentLives;
  bool decreasingAlpha;
  float maxSpeed;
  float deltaX;
  float deltaY;
  V2D initialPosition;
  int health;
};

struct EntityNode {
  Entity entity;
  EntityNode *next;
};

EntityType parseEntityType(const char *str);

void initEntity(Entity *entity);

void updateEntity(Entity *entity, PlayState* playState, GameContext *gameContext,
                  UserInput *userInput, GameMemory *gameMemory);

void drawEntity(Entity *entity, SDL_Renderer *renderer);

bool checkEntityCollision(Entity *entity1, Entity *entity2);

#endif //HELLO2SDL_ENTITY_H
