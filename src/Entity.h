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
  PLAYER_TYPE = 0,
  ENEMY_TYPE,
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
};

void resetEntity(Entity *entity);

void updateEntity(Entity *entity, GameContext *gameContext, UserInput *userInput,
                  PlayState* playState, GameMemory *gameMemory);

void drawEntity(Entity *entity, SDL_Renderer *renderer);

bool checkEntityCollision(Entity *entity1, Entity *entity2);

#endif //HELLO2SDL_ENTITY_H
