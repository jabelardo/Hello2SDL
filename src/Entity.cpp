//
// Created by Jose Gutierrez on 10/14/15.
//

#include "Entity.h"

bool
checkCollision(Entity *entity1, Entity *entity2) {
  float leftA = entity1->position.x;
  float rightA = entity1->position.x + entity1->bitmap.width;
  float topA = entity1->position.y;
  float bottomA = entity1->position.y + entity1->bitmap.height;
  //Calculate the sides of rect B
  float leftB = entity2->position.x;
  float rightB = entity2->position.x + entity2->bitmap.width;
  float topB = entity2->position.y;
  float bottomB = entity2->position.y + entity2->bitmap.height;
  //If any of the sides from A are outside of B
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}

