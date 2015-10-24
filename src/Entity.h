//
// Created by Jose Gutierrez on 10/23/15.
//

#ifndef HELLO2SDL_ENTITY_H
#define HELLO2SDL_ENTITY_H

#include "V2D.h"
#include "Bitmap.h"

enum EntityType {
  PLAYER_TYPE = 0,
  ENEMY_TYPE
};

struct Entity {
  EntityType type;
  V2D position;
  Bitmap bitmap;
  V2D velocity;
  V2D acceleration;
};

void updateEntity(Entity *entity, UserInput *userInput);

void drawEntity(Entity *entity, SDL_Renderer *renderer);

#endif //HELLO2SDL_ENTITY_H
