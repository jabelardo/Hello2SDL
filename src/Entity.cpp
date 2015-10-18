//
// Created by Jose Gutierrez on 10/14/15.
//

#include "Entity.h"

bool
checkCollision(Entity* entity1, Entity* entity2) {
  auto leftA = entity1->position.x;
  auto rightA = entity1->position.x + entity1->bitmap.width;
  auto topA = entity1->position.y;
  auto bottomA = entity1->position.y + entity1->bitmap.height;
  //Calculate the sides of rect B
  auto leftB = entity2->position.x;
  auto rightB = entity2->position.x + entity2->bitmap.width;
  auto topB = entity2->position.y;
  auto bottomB = entity2->position.y + entity2->bitmap.height;
  //If any of the sides from A are outside of B
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}

