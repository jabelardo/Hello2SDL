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
#include "PlayState.h"

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

const static int s_buffer = 4;

static bool
rectRect(SDL_Rect *A, SDL_Rect *B) {
  int aHBuf = A->h / s_buffer;
  int aWBuf = A->w / s_buffer;
  int bHBuf = B->h / s_buffer;
  int bWBuf = B->w / s_buffer;
  // if the bottom of A is less than the top of B - no collision
  if ((A->y + A->h) - aHBuf <= B->y + bHBuf) { return false; }
  // if the top of A is more than the bottom of B = no collision
  if (A->y + aHBuf >= (B->y + B->h) - bHBuf) { return false; }
  // if the right of A is less than the left of B - no collision
  if ((A->x + A->w) - aWBuf <= B->x + bWBuf) { return false; }
  // if the left of A is more than the right of B - no collision
  if (A->x + aWBuf >= (B->x + B->w) - bWBuf) { return false; }
  // otherwise there has been a collision
  return true;
}

V2D
getEntityScreenPosition(Entity *entity, V2D cameraPosition) {
  V2D result = entity->position - cameraPosition;
  return result;
}

V2D
getEntityDrawPosition(Entity *entity, V2D cameraPosition) {
  V2D result = getEntityScreenPosition(entity, cameraPosition)
             - V2D{(float) entity->bitmap.width / 2.f, (float) entity->bitmap.height / 2.f};

  return result;
}

void
drawEntity(Entity *entity, GameContext* gameContext, SDL_Renderer *renderer) {

  V2D drawPos = getEntityDrawPosition(entity, gameContext->cameraPosition);

  int x = (int) roundf(drawPos.x);
  int y = (int) roundf(drawPos.y);

  if ((x < -entity->bitmap.width / 2) ||
      (x > gameContext->gameWidth + entity->bitmap.width / 2) ||
      (y < -entity->bitmap.height / 2) ||
      (y > gameContext->gameHeight + entity->bitmap.height / 2)) {
    return;
  }
  switch (entity->type) {
    case PLAYER_TYPE: {

      drawBitmapEx(renderer, x, y, &entity->bitmap,
                   (entity->velocity.x < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
      break;
    }
    case ENEMY_BULLET_TYPE:
    case PLAYER_BULLET_TYPE:
    case GLIDER_TYPE:
    case SHOT_GLIDER_TYPE:
    case LEVEL_1_BOSS_TYPE:
    case ESKELETOR_TYPE:
    case TURRET_TYPE: {
      drawBitmap(renderer, x, y, &entity->bitmap);
      break;
    }
    case ROOF_TURRET_TYPE: {
      drawBitmap(renderer, x, y, &entity->bitmap, SDL_FLIP_VERTICAL);
      break;
    }
    case NULL_ENTITY_TYPE: {
      return;
    }
  }
  SDL_SetRenderDrawColor(renderer, 0,255,0,255);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = entity->bitmap.width;
  rect.h = entity->bitmap.height;
  SDL_RenderDrawRect(renderer, &rect);

  V2D screenPos =  getEntityScreenPosition(entity, gameContext->cameraPosition);

  SDL_RenderDrawLine(renderer, drawPos.x, drawPos.y, screenPos.x, screenPos.y);
}

void
initEntity(Entity *entity) {
  switch (entity->type) {
    case PLAYER_TYPE: {
      entity->halfCollisionDim = 15;
      entity->health = 1;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 5;
      entity->dyingTime = 100;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->bitmap.alpha = 255;
      entity->bitmap.r = 255;
      entity->bitmap.g = 255;
      entity->bitmap.b = 255;
      entity->invulnerableTime = 200;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 7;
      entity->bulletCounter = 0;
      break;
    }
    case GLIDER_TYPE: {
      entity->halfCollisionDim = 9;
      entity->deltaMovement = 60;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 3;
      entity->velocity = {-entity->maxSpeed, entity->maxSpeed / 2.f};
      entity->health = 1;
      entity->dyingTime = 25;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 0;
      entity->bulletCounter = 0;
      break;
    }
    case SHOT_GLIDER_TYPE: {
      entity->halfCollisionDim = 9;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 3;
      entity->velocity = {-entity->maxSpeed, 0};
      entity->health = 1;
      entity->dyingTime = 25;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 25;
      entity->bulletCounter = 0;
      break;
    }
    case ROOF_TURRET_TYPE:
    case TURRET_TYPE: {
      entity->halfCollisionDim = 14;
      entity->dyingTime = 1000;
      entity->health = 15;
      entity->bulletTime = 50;
      break;
    }
    case ESKELETOR_TYPE: {
      entity->halfCollisionDim = 8;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 3;
      entity->velocity = {0, 0};
      entity->health = 3;
      entity->dyingTime = 50;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 50;
      entity->bulletCounter = 0;
      break;
    }
    case PLAYER_BULLET_TYPE:
    case ENEMY_BULLET_TYPE: {
      entity->halfCollisionDim = 5;
      entity->health = 1;
      entity->dyingTime = 1;
      entity->dyingCounter = 0;
      break;
    }
    case LEVEL_1_BOSS_TYPE: {
      entity->halfCollisionDim = 28;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 2;
      entity->velocity = {-entity->maxSpeed, 0};
      entity->health = 100;
      entity->dyingTime = 100;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 100;
      entity->bulletCounter = 0;
      break;
    }
    case NULL_ENTITY_TYPE:break;
  }
}

EntityType parseEntityType(const char *str) {
  if (strcmp(str, "Player") == 0) {
    return PLAYER_TYPE;
  }
  if (strcmp(str, "Glider") == 0) {
    return GLIDER_TYPE;
  }
  if (strcmp(str, "ShotGlider") == 0) {
    return SHOT_GLIDER_TYPE;
  }
  if (strcmp(str, "Level1Boss") == 0) {
    return LEVEL_1_BOSS_TYPE;
  }
  if (strcmp(str, "Turret") == 0) {
    return TURRET_TYPE;
  }
  if (strcmp(str, "RoofTurret") == 0) {
    return ROOF_TURRET_TYPE;
  }
  if (strcmp(str, "Eskeletor") == 0) {
    return ESKELETOR_TYPE;
  }
  if (strcmp(str, "PlayerBullet") == 0) {
    return PLAYER_BULLET_TYPE;
  }
  if (strcmp(str, "EnemyBullet") == 0) {
    return ENEMY_BULLET_TYPE;
  }
  return NULL_ENTITY_TYPE;
}
