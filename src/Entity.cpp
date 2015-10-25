//
// Created by Jose Gutierrez on 10/24/15.
//
#ifdef __APPLE__
#include <SDL2/SDL_timer.h>
#else
#include <SDL_timer.h>
#endif

#include "Entity.h"
#include "RenderUtils.h"
#include "SharedDefinitions.h"

bool
checkEntityCollision(Entity *entity1, Entity *entity2) {
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


void
drawEntity(Entity *entity, SDL_Renderer *renderer) {
  switch (entity->type) {
    case ENEMY_TYPE:
    case PLAYER_TYPE: {
      if (entity->velocity.x > 0) {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                   SDL_FLIP_HORIZONTAL);
      } else {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap);
      }
    }
  }
}

void
updateEntity(Entity *entity, UserInput *userInput) {
  switch (entity->type) {
    case PLAYER_TYPE: {
      entity->bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity->bitmap.totalFrames);
      entity->velocity = V2D{0, 0};

      V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};
      entity->velocity = (target - entity->position) / 50;

      entity->velocity += entity->acceleration;
      entity->position += entity->velocity;
    }
      break;

    case ENEMY_TYPE: {
      entity->bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity->bitmap.totalFrames);
      if (entity->position.y < 0) {
        entity->velocity = V2D{2, .33f};
        entity->acceleration = V2D{0, .33f};

      } else if (entity->position.y > 400) {
        entity->velocity = V2D{2, -.33f};
        entity->acceleration = V2D{0, -.33f};
      }
      if (entity->position.x > 640) {
        entity->position.x = 0;
      }
      entity->velocity += entity->acceleration;
      entity->position += entity->velocity;
    }
      break;
  }
}