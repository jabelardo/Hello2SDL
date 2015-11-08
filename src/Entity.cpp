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

  if ((x < -entity->bitmap.width + 1) ||
      (x > gameContext->gameWidth + entity->bitmap.width - 1) ||
      (y < -entity->bitmap.height + 1) ||
      (y > gameContext->gameHeight + entity->bitmap.height - 1)) {
    return;
  }
  switch (entity->type) {
    case NULL_ENTITY_TYPE: {
      return;
    }
    case PLAYER_TYPE: {

      drawBitmapEx(renderer, x, y, &entity->bitmap,
                   (entity->velocity.x < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
      break;
    }
    case TILE_TYPE: {
      drawTile(renderer, x, y, &entity->bitmap);
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
  }
#ifdef DEBUG
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = entity->bitmap.width;
  rect.h = entity->bitmap.height;
  SDL_RenderDrawRect(renderer, &rect);

  V2D screenPos =  getEntityScreenPosition(entity, gameContext->cameraPosition);

  SDL_RenderDrawLine(renderer, drawPos.x, drawPos.y, screenPos.x, screenPos.y);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
#endif
}

void
initEntity(Entity *entity) {
  switch (entity->type) {
    case PLAYER_TYPE: {
      entity->halfCollisionWidth = 15;
      entity->halfCollisionHeight = 15;
      entity->health = 20;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 5;
      entity->dyingTime = 100;
      entity->dyingCounter = 0;
      entity->bitmap.currentColumn = 0;
      entity->bitmap.alpha = 255;
      entity->bitmap.r = 255;
      entity->bitmap.g = 255;
      entity->bitmap.b = 255;
      entity->invulnerableTime = 200;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 5;
      entity->bulletCounter = 0;
      break;
    }
    case GLIDER_TYPE: {
      entity->halfCollisionWidth = 9;
      entity->halfCollisionHeight = 9;
      entity->deltaMovement = 60;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 3;
      entity->velocity = {-entity->maxSpeed, entity->maxSpeed / 2.f};
      entity->health = 1;
      entity->dyingTime = 25;
      entity->dyingCounter = 0;
      entity->bitmap.currentColumn = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 0;
      entity->bulletCounter = 0;
      break;
    }
    case SHOT_GLIDER_TYPE: {
      entity->halfCollisionWidth = 9;
      entity->halfCollisionHeight = 9;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 3;
      entity->velocity = {-entity->maxSpeed, 0};
      entity->health = 1;
      entity->dyingTime = 25;
      entity->dyingCounter = 0;
      entity->bitmap.currentColumn = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 45;
      entity->bulletCounter = 0;
      break;
    }
    case ROOF_TURRET_TYPE:
    case TURRET_TYPE: {
      entity->halfCollisionWidth = 14;
      entity->halfCollisionHeight = 14;
      entity->dyingTime = 100;
      entity->health = 4;
      entity->bulletTime = 60;
      break;
    }
    case ESKELETOR_TYPE: {
      entity->halfCollisionWidth = 8;
      entity->halfCollisionHeight = 8;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 3;
      entity->velocity = {0, 0};
      entity->health = 2;
      entity->dyingTime = 50;
      entity->dyingCounter = 0;
      entity->bitmap.currentColumn = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 60;
      entity->bulletCounter = 0;
      break;
    }
    case PLAYER_BULLET_TYPE:
    case ENEMY_BULLET_TYPE: {
      entity->halfCollisionWidth = 5;
      entity->halfCollisionHeight = 5;
      entity->health = 1;
      entity->dyingTime = 1;
      entity->dyingCounter = 0;
      break;
    }
    case LEVEL_1_BOSS_TYPE: {
      entity->halfCollisionWidth = 28;
      entity->halfCollisionHeight = 28;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 2;
      entity->velocity = {-entity->maxSpeed, 0};
      entity->health = 25;
      entity->dyingTime = 100;
      entity->dyingCounter = 0;
      entity->bitmap.currentColumn = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 15;
      entity->bulletCounter = 0;
      break;
    }
    case NULL_ENTITY_TYPE:break;
    case TILE_TYPE:break;
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
