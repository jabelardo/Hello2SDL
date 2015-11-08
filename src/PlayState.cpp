//
// Created by Jose Gutierrez on 10/24/15.
//

#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include <assert.h>
#include <float.h>
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
  if (!loadTexture("SMALL_EXPLOSION", "smallexplosion.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("EXPLOSION", "explosion.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("LARGE_EXPLOSION", "largeexplosion.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("BOSS_EXPLOSION", "bossexplosion.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }

  TileMap *tileMap = RESERVE_MEMORY(&gameMemory->shortTimeMemory, TileMap);

  if (!loadTileMap(tileMap, "map1.tmx", gameContext, renderer, gameMemory,
                   platformConfig)) {
    return false;
  }
  playState->tileMapWidth = tileMap->width * tileMap->tileWidth;
  playState->scrollingBackground = tileMap->scrollingBackground;
  playState->entityList = tileMap->entityList;
  playState->player = 0;
  for (EntityNode *entityNode = playState->entityList; entityNode; entityNode = entityNode->next) {
    if (entityNode->entity.type == PLAYER_TYPE) {
      playState->player = &entityNode->entity;
      break;
    }
  }
  if (!playState->player) {
    return false;
  }

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
  playState->smallExplosionTexture = getTexture("SMALL_EXPLOSION", gameContext);
  if (!playState->smallExplosionTexture) {
    return false;
  }
  playState->explosionTexture = getTexture("EXPLOSION", gameContext);
  if (!playState->explosionTexture) {
    return false;
  }
  playState->largeExplosionTexture = getTexture("LARGE_EXPLOSION", gameContext);
  if (!playState->largeExplosionTexture) {
    return false;
  }
  playState->bossExplosionTexture = getTexture("BOSS_EXPLOSION", gameContext);
  if (!playState->bossExplosionTexture) {
    return false;
  }

  for (EntityNode *node = playState->entityList; node; node = node->next) {
    initEntity(&node->entity);
  }

  playState->currentLives = 3;

  return true;
}

void
renderPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer) {

  drawScrollingBackground(playState->scrollingBackground, gameContext, renderer);

  for (int i = 0; i < playState->currentLives; ++i) {
    drawTextureFrame(renderer, playState->liveTexture, i * 30, 0, 32, 30, 0, 0);
  }
  for (int i = 0; i < playState->player->health; ++i) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    int partWidth = 8;
    SDL_Rect rect = {(i * 4 * partWidth) + 1, 40, partWidth * 3, 5};
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  }

  for (EntityNode *node = playState->entityList; node; node = node->next) {
    drawEntity(&node->entity, gameContext, renderer);
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

  bullet->next = playState->entityList;

  playState->entityList = bullet;

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

  bullet->next = playState->entityList;

  playState->entityList = bullet;

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
updateCameraPosition(PlayState *playState, GameContext *gameContext) {
  if (gameContext->cameraPosition.x + gameContext->gameWidth < playState->tileMapWidth) {
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
      setEntityFlags(entity, DEC_ALPHA_FLAG);
    } else if (entity->bitmap.alpha == 125) {
      clearEntityFlags(entity, DEC_ALPHA_FLAG);
    }
    if (areEntityFlagsSet(entity, DEC_ALPHA_FLAG)) {
      entity->bitmap.alpha -= 10;
    } else {
      entity->bitmap.alpha += 10;
    }
    --entity->invulnerableCounter;
  }
  entity->bitmap.currentColumn = (SDL_GetTicks() / 100) % entity->bitmap.totalFrames;
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
    entity->bitmap.currentColumn = (SDL_GetTicks() / 100) % entity->bitmap.totalFrames;

  } else {
    entity->bitmap.currentColumn = (SDL_GetTicks() / 333) % entity->bitmap.totalFrames;
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

  if (handleDying(entity) && entity->dyingCounter == 1) {
    if (entity->type == LEVEL_1_BOSS_TYPE) {
      gameContext->isLevelCompleted = true;

    } else if (entity->type == PLAYER_TYPE) {
      if (playState->currentLives > 0) {
        resurrectPlayer(entity, playState);
      } else {
        gameContext->stateChange = GAME_OVER;
      }
    } else {
      entity->type = NULL_ENTITY_TYPE;
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
      if (entity->bulletCounter == 0 && entity->dyingCounter == 0) {
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
      if (entity->bulletCounter == 0 && entity->dyingCounter == 0) {
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
      if (entity->bulletCounter == 0 && entity->dyingCounter == 0) {
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
      if (entity->bulletCounter == 0 && entity->dyingCounter == 0) {
        entity->bulletCounter = entity->bulletTime;
        addEnemyBullet(playState, gameMemory, entity->position + V2D{-15, 10}, {-3, 0});
        addEnemyBullet(playState, gameMemory, entity->position + V2D{15, 10}, {3, 0});

      } else if (entity->bulletCounter > 0) {
        --entity->bulletCounter;
      }

      entity->bitmap.currentColumn = (SDL_GetTicks() / 150) % entity->bitmap.totalFrames;
      break;
    }
    case LEVEL_1_BOSS_TYPE: {

      if (screenPosition.x <= gameContext->gameWidth - 32) {

        if (entity->bulletCounter == 0 && entity->dyingCounter == 0) {
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
    case TILE_TYPE:
    case NULL_ENTITY_TYPE: {
      break;
    }
  }
}

bool
checkEntitiesOverlap(Entity *entity1, Entity *entity2) {

  if (entity1 == entity2) {
    return false;
  }

  if (entity1->type > entity2->type) {
    Entity *tmp = entity1;
    entity1 = entity2;
    entity2 = tmp;
  }

  if (entity1->type == NULL_ENTITY_TYPE) {
    return false;
  }

  float leftA = entity1->position.x - entity1->halfCollisionWidth;
  float rightA = entity1->position.x + entity1->halfCollisionWidth;
  float topA = entity1->position.y - entity1->halfCollisionHeight;
  float bottomA = entity1->position.y + entity1->halfCollisionHeight;

  float leftB = entity2->position.x - entity2->halfCollisionWidth;
  float rightB = entity2->position.x + entity2->halfCollisionWidth;
  float topB = entity2->position.y - entity2->halfCollisionHeight;
  float bottomB = entity2->position.y + entity2->halfCollisionHeight;

  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }

  return true;
}

void
handleEntitiesOverlap(PlayState *playState, Entity *entity1, Entity *entity2, V2D oldPos) {
  if (entity1->type > entity2->type) {
    Entity *tmp = entity1;
    entity1 = entity2;
    entity2 = tmp;
  }
  if (entity1->type == TILE_TYPE && !areEntityFlagsSet(entity1, DONT_COLLIDE_FLAG)) {
    switch (entity2->type) {
      case PLAYER_BULLET_TYPE:
      case ENEMY_BULLET_TYPE: {
        entity2->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        entity2->health = 0;
        break;
      }
      case PLAYER_TYPE: {
        entity2->position = oldPos;
        entity2->velocity.y *= -8;
        entity2->position += entity2->velocity;
        --entity2->health;
        break;
      }
      case GLIDER_TYPE:
      case ESKELETOR_TYPE: {
        entity2->position = oldPos;
        entity2->velocity.y *= -1;
        entity2->position += entity2->velocity;
        break;
      }
      case TILE_TYPE:
      case LEVEL_1_BOSS_TYPE:
      case ROOF_TURRET_TYPE:
      case TURRET_TYPE:
      case SHOT_GLIDER_TYPE:
      case NULL_ENTITY_TYPE:
      default: {
        break;
      }
    }
  } else if (entity1->type == PLAYER_BULLET_TYPE) {
    switch (entity2->type) {
      case PLAYER_TYPE: {
        break;
      }
      case ENEMY_BULLET_TYPE: {
        entity1->bitmap = entity2->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        entity1->health = entity2->health = 0;
        break;
      }
      case GLIDER_TYPE:
      case SHOT_GLIDER_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        --entity2->health;
        if (entity2->health < 1) {
          entity2->bitmap = {playState->explosionTexture, 40, 40, 9};
        }
        break;
      }
      case ESKELETOR_TYPE:
      case TURRET_TYPE:
      case ROOF_TURRET_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        --entity2->health;
        if (entity2->health < 1) {
          entity2->bitmap = {playState->largeExplosionTexture, 60, 60, 9};
        }
        break;
      }
      case LEVEL_1_BOSS_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        --entity2->health;
        if (entity2->health < 1) {
          entity2->bitmap = {playState->bossExplosionTexture, 180, 180, 9};
        }
        break;
      }
      default: {
        break;
      }
    }

  } else if (entity1->type == ENEMY_BULLET_TYPE) {
    switch (entity2->type) {
      case ENEMY_BULLET_TYPE: {
        entity1->bitmap = entity2->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        entity1->health = entity2->health = 0;
        break;
      }
      case PLAYER_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        --entity2->health;
        break;
      }
      case GLIDER_TYPE:
      case SHOT_GLIDER_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        break;
      }
      case ESKELETOR_TYPE:
      case TURRET_TYPE:
      case ROOF_TURRET_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        break;
      }
      case LEVEL_1_BOSS_TYPE: {
        entity1->health = 0;
        entity1->bitmap = {playState->smallExplosionTexture, 20, 20, 2};
        break;
      }
      default: {
        break;
      }
    }
  } else if (entity1->type == PLAYER_TYPE) {
    switch (entity2->type) {
      case GLIDER_TYPE:
      case SHOT_GLIDER_TYPE: {
        entity1->position = oldPos;
        entity1->velocity.y *= -8;
        entity1->position += entity1->velocity;
        --entity1->health;
        --entity2->health;
        break;
      }
      case ESKELETOR_TYPE:
      case TURRET_TYPE:
      case ROOF_TURRET_TYPE: {
        entity1->position = oldPos;
        entity1->velocity.y *= -8;
        entity1->position += entity1->velocity;
        --entity1->health;
        --entity2->health;
        break;
      }
      case LEVEL_1_BOSS_TYPE: {
        entity1->position = oldPos;
        entity1->velocity.y *= -8;
        entity1->position += entity1->velocity;
        --entity1->health;
        --entity2->health;
        break;
      }
      default: {
        break;
      }
    }
  }
}

void
doEntityMovement(PlayState *playState, Entity *entity, GameContext *gameContext) {

  V2D oldPos = entity->position;
  entity->position += entity->velocity;

  for (EntityNode *node = playState->entityList; node; node = node->next) {
    if (checkEntitiesOverlap(entity, &node->entity)) {
      handleEntitiesOverlap(playState, entity, &node->entity, oldPos);
    }
  }
}

void
moveEntity(PlayState *playState, Entity *entity, GameContext *gameContext, UserInput *userInput) {

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

          V2D entityVelocity = V2D{gameContext->scrollSpeed, 0}
                               + (target - screenPosition) / (entity->maxSpeed * entity->maxSpeed);

          if (lengthSquare(entityVelocity) > entity->maxSpeed * entity->maxSpeed) {
            normalize(entityVelocity);
            entityVelocity *= entity->maxSpeed;
          }

          entity->velocity = entityVelocity;

        } else {
          // if the player is dying
          entity->velocity += V2D{0, .05};
        }
        doEntityMovement(playState, entity, gameContext);
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
        doEntityMovement(playState, entity, gameContext);

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
        doEntityMovement(playState, entity, gameContext);
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
        if (entity->velocity.y == 0) {
          entity->velocity.y = entity->maxSpeed;
        }
        doEntityMovement(playState, entity, gameContext);

        if (entity->velocity.y < 0 && entity->position.y <= entity->initialPosition.y) {
          entity->velocity.y *= -1;
        }
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
          doEntityMovement(playState, entity, gameContext);
        }
      }
      break;
    }
    case TILE_TYPE:
    case NULL_ENTITY_TYPE: {
      break;
    }
  }
}

static void
updateEntities(PlayState *playState, EntityNode **entities, GameContext *gameContext, UserInput *userInput,
               GameMemory *gameMemory) {
  for (EntityNode **entityNode = entities; *entityNode;) {
    V2D position = getEntityScreenPosition(&(*entityNode)->entity, gameContext->cameraPosition);
    if (((*entityNode)->entity.type == PLAYER_BULLET_TYPE || (*entityNode)->entity.type == ENEMY_BULLET_TYPE) &&
        (position.x < -(*entityNode)->entity.bitmap.width ||
         position.x > gameContext->gameWidth + (*entityNode)->entity.bitmap.width ||
         position.y < -(*entityNode)->entity.bitmap.height ||
         position.y > gameContext->gameHeight + (*entityNode)->entity.bitmap.height ||
         (*entityNode)->entity.dyingCounter == 1)) {

      (*entityNode)->entity.dyingCounter = 0;

      EntityNode *freeEntity = *entityNode;

      // NOTE: must modify the pointer itself
      *entityNode = (*entityNode)->next;

      freeEntity->next = playState->freeEntities;
      playState->freeEntities = freeEntity;
      assert((*entityNode == 0) || (*entityNode != (*entityNode)->next));

    } else {
      updateEntity(playState, &(*entityNode)->entity, gameContext, userInput, gameMemory);
      moveEntity(playState, &(*entityNode)->entity, gameContext, userInput);

      entityNode = &(*entityNode)->next;
      assert((*entityNode == 0) || (*entityNode != (*entityNode)->next));
    }
  }
}

void
updatePlayState(PlayState *playState, GameContext *gameContext, UserInput *userInput,
                GameMemory *gameMemory) {

  updateScrollingBackground(playState->scrollingBackground, gameContext);

  updateCameraPosition(playState, gameContext);

  updateEntities(playState, &playState->entityList, gameContext, userInput, gameMemory);

}
