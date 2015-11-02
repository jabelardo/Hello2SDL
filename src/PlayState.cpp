//
// Created by Jose Gutierrez on 10/24/15.
//

#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include <assert.h>
#include "PlayState.h"
#include "Entity.h"
#include "TextureStorage.h"
#include "MemoryPartition.h"
#include "TileMap.h"
#include "SharedDefinitions.h"
#include "RenderUtils.h"
#include "ScrollingBackground.h"

bool
initPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer,
              GameMemory *gameMemory, PlatformConfig *platformConfig) {

  if (!loadTexture("BULLET1", "bullet1.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("BULLET2", "bullet2.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("BULLET3", "bullet3.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("LIVES", "lives.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }

  playState->tileMap = RESERVE_MEMORY(&gameMemory->permanentMemory, TileMap);

  if (!initTileMap(playState->tileMap, "map1.tmx", gameContext, renderer, gameMemory,
                   platformConfig)) {
    return false;
  }

  playState->playerBullets = 0;
  playState->enemyBullets = 0;
  playState->freeEntities = 0;

  return true;
}

bool
startGame(PlayState *playState, GameContext *gameContext) {
  playState->liveTexture = getTexture("LIVES", gameContext);
  if (!playState->liveTexture) {
    return false;
  }
  playState->bullet1Texture = getTexture("BULLET1", gameContext);
  if (!playState->bullet1Texture) {
    return false;
  }
  playState->bullet2Texture = getTexture("BULLET2", gameContext);
  if (!playState->bullet2Texture) {
    return false;
  }
  for (TileLayer *tileLayer = playState->tileMap->tileLayerList; tileLayer;
       tileLayer = tileLayer->next) {
    tileLayer->position = {0, 0};
  }

  initEntity(playState->tileMap->player);

  playState->currentLives = 3;

  for (ObjectLayer *objNode = playState->tileMap->objectLayerList; objNode;
       objNode = objNode->next) {
    for (EntityNode *node = objNode->entityList; node; node = node->next) {
      initEntity(&node->entity);
    }
  }
  return true;
}

void
renderPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer) {

  drawTileMap(playState->tileMap, gameContext, renderer);

  for (int i = 0; i < playState->currentLives; ++i) {
    drawTextureFrame(renderer, playState->liveTexture, i * 30, 0, 32, 30, 0, 0);
  }

  for (EntityNode *bullet = playState->playerBullets; bullet; bullet = bullet->next) {
    drawEntity(&bullet->entity, gameContext, renderer);
  }

  for (EntityNode *bullet = playState->enemyBullets; bullet; bullet = bullet->next) {
    drawEntity(&bullet->entity, gameContext, renderer);
  }
}

void
addPlayerBullet(PlayState *playState, GameMemory *gameMemory, V2D position, V2D velocity) {

  EntityNode *bullet = playState->freeEntities;

  if (bullet) {
    playState->freeEntities = bullet->next;
  } else {
    bullet = RESERVE_MEMORY(&gameMemory->permanentMemory, EntityNode);
  }

  bullet->next = playState->playerBullets;

  playState->playerBullets = bullet;

  bullet->entity = {PLAYER_BULLET_TYPE,
                    position,
                    {playState->bullet1Texture, 11, 11, 1},
                    velocity};

  initEntity(&bullet->entity);

  assert(bullet != bullet->next);
}

void
addEnemyBullet(PlayState *playState, GameMemory *gameMemory, V2D position, V2D velocity) {

  EntityNode *bullet = playState->freeEntities;

  if (bullet) {
    playState->freeEntities = bullet->next;
  } else {
    bullet = RESERVE_MEMORY(&gameMemory->permanentMemory, EntityNode);
  }

  bullet->next = playState->enemyBullets;

  playState->enemyBullets = bullet;

  bullet->entity = {ENEMY_BULLET_TYPE,
                    position,
                    {playState->bullet2Texture, 16, 16, 1},
                    velocity};

  initEntity(&bullet->entity);

  assert(bullet != bullet->next);
}

#if 0
void
addEnemy(PlayState *playState, GameContext *gameContext, GameMemory *gameMemory) {

  EntityNode* enemy = playState->freeEntities;

  if (enemy) {
    playState->freeEntities = enemy->next;
  } else {
    enemy = RESERVE_MEMORY(&gameMemory->permanentMemory, EntityNode);
  }

  enemy->next = playState->enemies;

  playState->enemies = enemy;

  int yPos = rand() % (gameContext->gameHeight / 2);
  playState->enemies->entity = {GLIDER_TYPE, {600, (float) yPos},
                                {playState->gliderTexture, 38, 34, 1}};

  initEntity(&enemy->entity);

  assert(enemy != enemy->next);
}
#endif

void
updateCameraPosition(TileMap *tileMap, GameContext *gameContext) {
  if (gameContext->cameraPosition.x + gameContext->gameWidth <
      tileMap->width * tileMap->tileWidth) {
    gameContext->cameraPosition += {gameContext->scrollSpeed, 0};
  }
}

void
resurrectPlayer(Entity *entity, PlayState *playState) {
  initEntity(entity);
  --playState->currentLives;
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
  entity->bitmap.currentFrame = (SDL_GetTicks() / 100) % entity->bitmap.totalFrames;
}

bool
handleDying(Entity *entity) {
  if (entity->dyingCounter == 0) {
    return false;
  }
  if (entity->type == PLAYER_TYPE) {
    entity->bitmap.r = 255;
    entity->bitmap.g = 0;
    entity->bitmap.b = 0;
    entity->bitmap.currentFrame = (SDL_GetTicks() / 100) % entity->bitmap.totalFrames;
    entity->bitmap.currentFrame = (SDL_GetTicks() / 333) % entity->bitmap.totalFrames;

  } else {
    entity->bitmap.currentFrame = (SDL_GetTicks() / 333) % entity->bitmap.totalFrames;
  }
  if (entity->dyingCounter > 1) {
    --entity->dyingCounter;
  }
  return true;
}

void
updateEntity(PlayState *playState, Entity *entity, GameContext *gameContext, UserInput *userInput,
             GameMemory *gameMemory) {

  assert(entity->dyingCounter >= 0);

  if (entity->health < 1 && entity->dyingCounter < 1) {
    entity->dyingCounter = entity->dyingTime;
  }

  V2D screenPosition = getEntityScreenPosition(entity, gameContext->cameraPosition);

  if (entity->type != PLAYER_TYPE) {
    if ((screenPosition.x > gameContext->gameWidth + entity->bitmap.width) ||
        (screenPosition.x < -entity->bitmap.width)) {
      return;
    }
  }

  if (handleDying(entity)) {
    if ((entity->type == LEVEL_1_BOSS_TYPE) && (entity->dyingCounter == 1)) {
      gameContext->isLevelCompleted = true;

    } else if ((entity->type == PLAYER_TYPE) && (entity->dyingCounter == 1)) {
      if (playState->currentLives > 0) {
        resurrectPlayer(entity, playState);
      } else {
        gameContext->stateChange = GAME_OVER;
      }
    }
    return;
  }

  switch (entity->type) {
    case PLAYER_TYPE: {
      handlePlayerAnimation(entity);

      if (userInput->mouseButtonLeft.endedDown) {
        if (entity->bulletCounter == 0) {
          entity->bulletCounter = entity->bulletTime;
          if (entity->velocity.x >= 0) {
            addPlayerBullet(playState, gameMemory,
                            {entity->position.x + 40, entity->position.y + 23},
                            {10, 0});
          } else {
            addPlayerBullet(playState, gameMemory,
                            {entity->position.x - 40, entity->position.y + 23},
                            {-10, 0});
          }
        }
      } else if (entity->bulletCounter > 0) {
        --entity->bulletCounter;
      }
      break;
    }
    case GLIDER_TYPE: {
      break;
    }
    case SHOT_GLIDER_TYPE: {
      if (entity->bulletCounter == 0) {
        entity->bulletCounter = entity->bulletTime;
        addEnemyBullet(playState, gameMemory, entity->position + V2D{-20, 0}, {-10, 0});

      } else if (entity->bulletCounter > 0) {
        --entity->bulletCounter;
      }
      break;
    }
    case PLAYER_BULLET_TYPE:
    case ENEMY_BULLET_TYPE: {
      break;
    }
    case TURRET_TYPE: {
      if (entity->bulletCounter == 0) {
        entity->bulletCounter = entity->bulletTime;
        addEnemyBullet(playState, gameMemory, entity->position + V2D{-20, -20}, {-3, -3});
        addEnemyBullet(playState, gameMemory, entity->position + V2D{0, -20}, {0, -3});
        addEnemyBullet(playState, gameMemory, entity->position + V2D{20, -20}, {3, -3});

      } else if (entity->bulletCounter > 0) {
        --entity->bulletCounter;
      }
      break;
    }
    case ROOF_TURRET_TYPE: {
      if (entity->bulletCounter == 0) {
        entity->bulletCounter = entity->bulletTime;
        addEnemyBullet(playState, gameMemory, entity->position + V2D{-20, 20}, {-3, 3});
        addEnemyBullet(playState, gameMemory, entity->position + V2D{0, 20}, {0, 3});
        addEnemyBullet(playState, gameMemory, entity->position + V2D{20, 20}, {3, 3});

      } else if (entity->bulletCounter > 0) {
        --entity->bulletCounter;
      }
      break;
    }
    case ESKELETOR_TYPE: {
      if (entity->bulletCounter == 0) {
        entity->bulletCounter = entity->bulletTime;
        addEnemyBullet(playState, gameMemory, entity->position + V2D{0, 10}, {-3, 0});
        addEnemyBullet(playState, gameMemory, entity->position + V2D{0, 10}, {3, 0});

      } else if (entity->bulletCounter > 0) {
        --entity->bulletCounter;
      }

      entity->bitmap.currentFrame = (SDL_GetTicks() / 150) % entity->bitmap.totalFrames;
      break;
    }
    case LEVEL_1_BOSS_TYPE: {

      if (screenPosition.x <= gameContext->gameWidth - 32) {

        if (entity->bulletCounter == 0) {
          entity->bulletCounter = entity->bulletTime;

          addEnemyBullet(playState, gameMemory, entity->position + V2D{-80, -97}, {-10, 0});
          addEnemyBullet(playState, gameMemory, entity->position + V2D{-80, -73}, {-10, 0});

          addEnemyBullet(playState, gameMemory, entity->position + V2D{-80, 73}, {-10, 0});
          addEnemyBullet(playState, gameMemory, entity->position + V2D{-80, 97}, {-10, 0});

        } else if (entity->bulletCounter > 0) {
          --entity->bulletCounter;
        }
      }
      break;
    }
    case NULL_ENTITY_TYPE: {
      break;
    }
  }
}

void
moveEntity(Entity *entity, GameContext *gameContext, UserInput *userInput) {

  V2D screenPosition = getEntityScreenPosition(entity, gameContext->cameraPosition);

  if (entity->type != PLAYER_TYPE) {
    if ((screenPosition.x > gameContext->gameWidth + entity->bitmap.width) ||
        (screenPosition.x < -entity->bitmap.width)) {
      return;
    }
  }
  switch (entity->type) {
    case PLAYER_TYPE: {
      // if the level is complete then fly off the screen
      if (gameContext->isLevelCompleted) {
        if (screenPosition.x >= gameContext->gameWidth + entity->bitmap.width) {
          ++gameContext->currentLevel;
          gameContext->stateChange = BETWEEN_LEVEL;

        } else {
          entity->velocity = V2D{3, 0};
          entity->position += entity->velocity;
        }
      } else {
        // if the player is not dying then update it normally
        if (entity->dyingCounter == 0) {
          V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};

          V2D entityVelocity = (target - screenPosition)
                               / (entity->maxSpeed * entity->maxSpeed);

          if (lengthSquare(entityVelocity) > entity->maxSpeed * entity->maxSpeed) {
            normalize(entityVelocity);
            entityVelocity *= entity->maxSpeed;
          }

          entity->velocity = entityVelocity + V2D{gameContext->scrollSpeed, 0};

        } else {
          // if the player is dying
          entity->velocity += V2D{0, .05};
        }
        entity->position += entity->velocity;
      }
      // change the angle with the velocity to give the impression of a moving helicopter
      if (entity->velocity.x < 0) {
        entity->bitmap.angle = -10.0;
      } else if (entity->velocity.y > 0) {
        entity->bitmap.angle = 10.0;
      } else {
        entity->bitmap.angle = 0.0;
      }
      break;
    }
    case GLIDER_TYPE: {
      if (entity->dyingCounter == 0) {
        if (entity->position.y >= entity->initialPosition.y + entity->deltaMovement) {
          entity->velocity.y = -entity->maxSpeed;

        } else if (entity->position.y <= entity->initialPosition.y - entity->deltaMovement) {
          entity->velocity.y = entity->maxSpeed;
        }
        entity->position += entity->velocity;

        // TODO: improve movement with collision detection
        if (entity->position.y < 0) {
          entity->position.y = 0;
          entity->velocity.y = entity->maxSpeed;

        } else if (entity->position.y > gameContext->gameHeight) {
          entity->position.y = gameContext->gameHeight;
          entity->velocity.y = -entity->maxSpeed;
        }

      }
      break;
    }
    case SHOT_GLIDER_TYPE: {
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
      break;
    }
    case ROOF_TURRET_TYPE: {
      break;
    }
    case ESKELETOR_TYPE: {
      if (entity->dyingCounter == 0) {
        entity->velocity.y = entity->maxSpeed;
        entity->position += entity->velocity;

        // TODO improve or remove when region simulation is completed
        if (entity->position.y > gameContext->gameHeight) {
          entity->position.y = entity->initialPosition.y;
        }

      } else {
        entity->velocity.y = 0;
      }
      break;
    }
    case LEVEL_1_BOSS_TYPE: {

      if (screenPosition.x > gameContext->gameWidth - 32) {
        entity->velocity.x = -entity->maxSpeed;
        entity->position += entity->velocity;

      } else {
        if (entity->dyingCounter == 0) {
          entity->velocity.x = 0;
          if (entity->velocity.y == 0) {
            entity->velocity.y = (random() % 2) ? entity->maxSpeed : -entity->maxSpeed;
          }

          if (screenPosition.y + 32 + entity->bitmap.height / 2 >= gameContext->gameHeight) {
            entity->velocity.y = -entity->maxSpeed;

          } else if (screenPosition.y - entity->bitmap.height / 2 <= 0) {
            entity->velocity.y = entity->maxSpeed;
          }
          entity->position += entity->velocity;
        }
      }
      break;
    }
    case NULL_ENTITY_TYPE: {
      break;
    }
  }
}

static void
updateTransientEntities(PlayState *playState, EntityNode **entities, GameContext *gameContext,
                        UserInput *userInput, GameMemory *gameMemory) {
  for (EntityNode **entityNode = entities; *entityNode;) {
    V2D position = getEntityScreenPosition(&(*entityNode)->entity, gameContext->cameraPosition);
    if (position.x < -(*entityNode)->entity.bitmap.width ||
        position.x > gameContext->gameWidth + (*entityNode)->entity.bitmap.width ||
        position.y < -(*entityNode)->entity.bitmap.height ||
        position.y > gameContext->gameHeight + (*entityNode)->entity.bitmap.height ||
        (*entityNode)->entity.dyingCounter == 1) {

      (*entityNode)->entity.dyingCounter = 0;

      EntityNode *freeEntity = *entityNode;

      // NOTE: must modify the pointer itself
      *entityNode = (*entityNode)->next;

      freeEntity->next = playState->freeEntities;
      playState->freeEntities = freeEntity;
      assert((*entityNode == 0) || (*entityNode != (*entityNode)->next));

    } else {
      updateEntity(playState, &(*entityNode)->entity, gameContext, userInput, gameMemory);
      entityNode = &(*entityNode)->next;
      assert((*entityNode == 0) || (*entityNode != (*entityNode)->next));
    }
  }
}

void
updatePlayState(PlayState *playState, GameContext *gameContext, UserInput *userInput,
                GameMemory *gameMemory) {

  updateScrollingBackground(playState->tileMap->scrollingBackground, gameContext);

  updateCameraPosition(playState->tileMap, gameContext);

  updateEntity(playState, playState->tileMap->player, gameContext, userInput, gameMemory);

  updateTransientEntities(playState, &playState->playerBullets, gameContext, userInput,
                          gameMemory);

  for (ObjectLayer *objNode = playState->tileMap->objectLayerList; objNode; objNode = objNode->next) {
    for (EntityNode *node = objNode->entityList; node; node = node->next) {
      updateEntity(playState, &node->entity, gameContext, userInput, gameMemory);
    }
  }

  updateTransientEntities(playState, &playState->enemyBullets, gameContext, userInput,
                          gameMemory);

  moveEntity(playState->tileMap->player, gameContext, userInput);

  for (EntityNode *node = playState->playerBullets; node; node = node->next) {
    moveEntity(&node->entity, gameContext, userInput);
  }

  for (ObjectLayer *objNode = playState->tileMap->objectLayerList; objNode; objNode = objNode->next) {
    for (EntityNode *node = objNode->entityList; node; node = node->next) {
      moveEntity(&node->entity, gameContext, userInput);
    }
  }

  for (EntityNode *node = playState->enemyBullets; node; node = node->next) {
    moveEntity(&node->entity, gameContext, userInput);
  }




//  Entity *player = playState->tileMap->player;
//  for (EntityNode *bullet = playState->playerBullets; bullet; bullet = bullet->next) {
//    if (checkEntityCollision(player, &bullet->entity)) {
//
//    }
//  }

//  for (EntityNode *enemy = playState->enemies; enemy; enemy = enemy->next) {
//    if (checkEntityCollision(player, &enemy->entity)) {
//
//    }
//  }
//  for (EntityNode *enemy = playState->enemies; enemy; enemy = enemy->next) {
//    for (EntityNode *bullet = playState->playerBullets; bullet; bullet = bullet->next) {
//      if (checkEntityCollision(&enemy->entity, &bullet->entity)) {
//        if (enemy->entity.health > 0) {
//          --enemy->entity.health;
//        }
//        if (bullet->entity.health > 0) {
//          --bullet->entity.health;
//        }
//      }
//    }
//  }
}
