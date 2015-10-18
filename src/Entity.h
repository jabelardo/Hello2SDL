//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_ENTITY_H
#define HELLO2SDL_ENTITY_H

#include "Bitmap.h"
#include "V2D.h"

struct Entity {
  V2D position;
  Bitmap bitmap;
  V2D velocity;
  V2D acceleration;
};

bool checkCollision(Entity* entity1, Entity* entity2);

#endif //HELLO2SDL_ENTITY_H
