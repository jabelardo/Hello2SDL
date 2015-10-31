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

void
drawEntity(Entity *entity, SDL_Renderer *renderer) {
  switch (entity->type) {
    case PLAYER_TYPE: {
      if (entity->velocity.x < 0) {
        drawBitmapEx(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                     SDL_FLIP_HORIZONTAL);

      } else {
        drawBitmapEx(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap);

      }
      break;
    }
    case ENEMY_BULLET_TYPE:
    case PLAYER_BULLET_TYPE: {
      if (entity->velocity.x > 0) {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
                   SDL_FLIP_HORIZONTAL);

      } else {
        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap);

      }
      break;
    }
    case GLIDER_TYPE: {
//      if (entity->velocity.x < 0) {
//        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap,
//                   SDL_FLIP_HORIZONTAL);
//
//      } else {
//        drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap);
//      }
//      break;
    }
    case SHOT_GLIDER_TYPE:
    case LEVEL_1_BOSS_TYPE:
    case ROOF_TURRET_TYPE:
    case ESKELETOR_TYPE:
    case TURRET_TYPE: {
      drawBitmap(renderer, (int) entity->position.x, (int) entity->position.y, &entity->bitmap);
      break;
    }
    case NULL_ENTITY_TYPE:break;
  }
}


void
doDyingAnimation(Entity *entity) {
  entity->bitmap.currentFrame = int(((SDL_GetTicks() / (1000 / 3)) % entity->bitmap.totalFrames));
  if (entity->dyingCounter > 1) {
    --entity->dyingCounter; //simple counter, fine with fixed frame rate
  }
}

void
initEntity(Entity *entity) {
  switch (entity->type) {
    case PLAYER_TYPE: {
      entity->health = 1;
      entity->position.x = 10;
      entity->position.y = 200;
      entity->dyingTime = 160;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->bitmap.alpha = 255;
      entity->bitmap.r = 255;
      entity->bitmap.g = 255;
      entity->bitmap.b = 255;
      entity->invulnerableTime = 320;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 7;
      entity->bulletCounter = 0;
      break;
    }
    case GLIDER_TYPE: {
      entity->deltaY = 60;
      entity->initialPosition = entity->position;
      entity->maxSpeed = 2;
      entity->velocity = {-entity->maxSpeed, entity->maxSpeed / 2.f};
      entity->health = 1;
      entity->dyingTime = 25;
      entity->dyingCounter = 0;
      entity->bitmap.currentFrame = 0;
      entity->invulnerableTime = 0;
      entity->invulnerableCounter = 0;
      entity->bulletTime = 15;
      entity->bulletCounter = 0;
      break;
    }
    case TURRET_TYPE: {
      entity->dyingTime = 1000;
      entity->health = 15;
      entity->bulletTime = 50;
      break;
    }
    case PLAYER_BULLET_TYPE:
    case ENEMY_BULLET_TYPE: {
      entity->health = 1;
      entity->dyingTime = 1;
      entity->dyingCounter = 0;
      break;
    }
    case SHOT_GLIDER_TYPE:break;
    case LEVEL_1_BOSS_TYPE:break;
    case ROOF_TURRET_TYPE:break;
    case ESKELETOR_TYPE:break;
    case NULL_ENTITY_TYPE:break;
  }
}

void
resurrectPlayer(Entity *entity) {
  initEntity(entity);
  --entity->currentLives;
  entity->invulnerableCounter = entity->invulnerableTime;
}

void
handlePlayerAnimation(Entity *entity) {
  // if the player is invulnerable we can flash its alpha to let people know
  if (entity->invulnerableCounter == 1) {
    entity->invulnerableCounter = 0;
    entity->bitmap.alpha = 255;

  } else if (entity->invulnerableCounter > 1) {
    // otherwise, flash the alpha on and off

    if (entity->bitmap.alpha == 255) {
      entity->decreasingAlpha = true;
    } else if (entity->bitmap.alpha == 125) {
      entity->decreasingAlpha = false;
    }
    if (entity->decreasingAlpha) {
      entity->bitmap.alpha -= 10;
    } else {
      entity->bitmap.alpha += 10;
    }
    --entity->invulnerableCounter;
  }

  // player is dying
  if (entity->dyingCounter > 0) {
    entity->bitmap.r = 255;
    entity->bitmap.g = 0;
    entity->bitmap.b = 0;
    --entity->dyingCounter;

  } else {
    // if the player is not dead then we can change the angle with the velocity to give the
    // impression of a moving helicopter
    if (entity->velocity.x < 0) {
      entity->bitmap.angle = -10.0;
    } else if (entity->velocity.y > 0) {
      entity->bitmap.angle = 10.0;
    } else {
      entity->bitmap.angle = 0.0;
    }
  }
  entity->bitmap.currentFrame = (SDL_GetTicks() / 100) % entity->bitmap.totalFrames;
}

void
scroll(Entity *entity, float scrollSpeed) {
  entity->velocity = {-scrollSpeed, 0};
  entity->position += entity->velocity;
}

void
updateEntity(Entity *entity, PlayState* playState, GameContext *gameContext, UserInput *userInput,
             GameMemory *gameMemory) {

  if (entity->health < 1 && entity->dyingCounter < 1) {
    entity->dyingCounter = entity->dyingTime;
  }
  switch (entity->type) {
    case PLAYER_TYPE: {
      // if the level is complete then fly off the screen
      if (gameContext->isLevelCompleted) {
        if (entity->position.x >= gameContext->gameWidth) {
          ++gameContext->currentLevel;
          gameContext->stateChange = BETWEEN_LEVEL;
          gameContext->levelComplete = false;

        } else {
          entity->velocity = V2D{3, 0};
          entity->position += entity->velocity;
          handlePlayerAnimation(entity);
        }
      } else {
        // if the player is not doing its death animation then update it normally
        if (entity->dyingCounter == 0) {
          V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};
          entity->velocity = (target - entity->position) / 50;

        } else if (entity->dyingCounter == 1) {
          if (entity->currentLives > 0) {
            resurrectPlayer(entity);
          } else {
            gameContext->stateChange = GAME_OVER;
            break;
          }

        } else {
          // if the player is doing the death animation
          entity->velocity += V2D{0, .05};
        }
        entity->position += entity->velocity;
        handlePlayerAnimation(entity);

        if (userInput->mouseButtonLeft.endedDown) {
          if (entity->bulletCounter == 0) {
            entity->bulletCounter = entity->bulletTime;
            if (entity->velocity.x >= 0) {
              addPlayerBullet(playState, gameMemory,
                              {entity->position.x + 90, entity->position.y + 12},
                              {10, 0});
            } else {
              addPlayerBullet(playState, gameMemory,
                              {entity->position.x + 40, entity->position.y + 12},
                              {-10, 0});
            }
          }
        } else if (entity->bulletCounter > 0) {
          --entity->bulletCounter;
        }
      }
      break;
    }
    case GLIDER_TYPE: {
      if (entity->dyingCounter == 0) {

        if (entity->position.y < entity->initialPosition.y - entity->deltaY) {
          entity->velocity.y = entity->maxSpeed;

        } else if (entity->position.y > entity->initialPosition.y + entity->deltaY) {
          entity->velocity.y = -entity->maxSpeed;
        }
        entity->position += entity->velocity;

        if (entity->position.y < 0) {
          entity->position.y = 0;
          entity->velocity.y = entity->maxSpeed;

        } else if (entity->position.y > gameContext->gameHeight) {
          entity->position.y = gameContext->gameHeight;
          entity->velocity.y = -entity->maxSpeed;
        }

      } else {
        scroll(entity, (float) gameContext->scrollSpeed);
        doDyingAnimation(entity);
      }
      break;
    }
    case PLAYER_BULLET_TYPE:
    case ENEMY_BULLET_TYPE: {
      if (entity->dyingCounter == 0) {
        entity->position += entity->velocity;
      }
      break;
    }
    case TURRET_TYPE: {
      if (entity->dyingCounter == 0) {
        scroll(entity, (float) gameContext->scrollSpeed);

        if (entity->bulletCounter == 0) {
          entity->bulletCounter = entity->bulletTime;
          addEnemyBullet(playState, gameMemory, entity->position,  {-3, -3});
          addEnemyBullet(playState, gameMemory, entity->position + V2D{20, 0}, {0, -3});
          addEnemyBullet(playState, gameMemory, entity->position + V2D{40, 0}, {3, -3});


        } else if (entity->bulletCounter > 0) {
          --entity->bulletCounter;
        }
      } else {
        scroll(entity, gameContext->scrollSpeed);
        doDyingAnimation(entity);
      }
      break;
    }
    case SHOT_GLIDER_TYPE:break;
    case LEVEL_1_BOSS_TYPE:break;
    case ROOF_TURRET_TYPE:break;
    case ESKELETOR_TYPE:break;
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
