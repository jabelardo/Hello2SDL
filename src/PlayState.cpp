//
// Created by Jose Gutierrez on 10/24/15.
//

#include <assert.h>
#include "PlayState.h"
#include "Entity.h"
#include "TextureStorage.h"
#include "MemoryPartition.h"
#include "TileMap.h"
#include "SharedDefinitions.h"
#include "RenderUtils.h"

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
//  if (!loadTexture("GLIDER", "enemy1.png", platformConfig->resourcePath, renderer,
//                   gameContext, gameMemory)) {
//    return false;
//  }

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
//  playState->gliderTexture = getTexture("GLIDER", gameContext);
//  if (!playState->gliderTexture) {
//    return false;
//  }
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

  playState->tileMap->player->position =
      playState->tileMap->playerInitialPosition;

  initEntity(playState->tileMap->player);
  playState->tileMap->player->currentLives = 3;

  for (ObjectLayer *objNode = playState->tileMap->objectLayerList; objNode;
       objNode = objNode->next) {
    for (EntityNode *node = objNode->entityList; node; node = node->next) {
      initEntity(&node->entity);
    }
  }

  return true;
}

void
renderPlayState(PlayState *playState, SDL_Renderer *renderer) {

  drawTileMap(playState->tileMap, renderer);

  for (int i = 0; i < playState->tileMap->player->currentLives; ++i) {
    drawTextureFrame(renderer, playState->liveTexture, i * 30, 0, 32, 30, 0, 0);
  }

//  for (EntityNode *enemy = playState->enemies; enemy; enemy = enemy->next) {
//    drawEntity(&enemy->entity, renderer);
//  }

  for (EntityNode *bullet = playState->playerBullets; bullet; bullet = bullet->next) {
    drawEntity(&bullet->entity, renderer);
  }

  for (EntityNode *bullet = playState->enemyBullets; bullet; bullet = bullet->next) {
    drawEntity(&bullet->entity, renderer);
  }
}

void
addPlayerBullet(PlayState* playState, GameMemory *gameMemory, V2D position, V2D velocity) {

  EntityNode* bullet = playState->freeEntities;

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
addEnemyBullet(PlayState* playState, GameMemory *gameMemory, V2D position, V2D velocity) {

  EntityNode* bullet = playState->freeEntities;

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

static void
updateEntities(EntityNode** entities, PlayState *playState, GameContext *gameContext,
               UserInput *userInput, GameMemory *gameMemory) {
  for (EntityNode **entityNode = entities; *entityNode;) {
    if ((*entityNode)->entity.position.x < 0 ||
        (*entityNode)->entity.position.x > gameContext->gameWidth ||
        (*entityNode)->entity.position.y < 0 ||
        (*entityNode)->entity.position.y > gameContext->gameHeight ||
        (*entityNode)->entity.dyingCounter == 1) {

      (*entityNode)->entity.dyingCounter = 0;

      EntityNode* freeEntity = *entityNode;

      // NOTE: must modify the pointer itself
      *entityNode = (*entityNode)->next;

      freeEntity->next = playState->freeEntities;
      playState->freeEntities = freeEntity;
      assert((*entityNode == 0) || (*entityNode != (*entityNode)->next));

    } else {
      updateEntity(&(*entityNode)->entity, playState, gameContext, userInput, gameMemory);
      entityNode = &(*entityNode)->next;
      assert((*entityNode == 0) || (*entityNode != (*entityNode)->next));
    }
  }
}

void
addEnemy(PlayState *playState, GameContext *gameContext, GameMemory *gameMemory) {

//  EntityNode* enemy = playState->freeEntities;
//
//  if (enemy) {
//    playState->freeEntities = enemy->next;
//  } else {
//    enemy = RESERVE_MEMORY(&gameMemory->permanentMemory, EntityNode);
//  }
//
//  enemy->next = playState->enemies;
//
//  playState->enemies = enemy;
//
//  int yPos = rand() % (gameContext->gameHeight / 2);
//  playState->enemies->entity = {GLIDER_TYPE, {600, (float) yPos},
//                                {playState->gliderTexture, 38, 34, 1}};
//
//  initEntity(&enemy->entity);
//
//  assert(enemy != enemy->next);
}

void
updatePlayState(PlayState *playState, GameContext *gameContext, UserInput *userInput,
                GameMemory *gameMemory) {
  if (userInput->back.endedDown) {
    gameContext->stateChange = PAUSE_MENU;
    return;
  }

//  if (!playState->enemies) {
//    addEnemy(playState, gameContext, gameMemory);
//  }

  updateTileMap(playState->tileMap, playState, gameContext, userInput, gameMemory);

//  updateEntities(&playState->enemies, playState, gameContext, userInput, gameMemory);

  updateEntities(&playState->playerBullets, playState, gameContext, userInput, gameMemory);

  updateEntities(&playState->enemyBullets, playState, gameContext, userInput, gameMemory);

  Entity *player = playState->tileMap->player;
  for (EntityNode *bullet = playState->playerBullets; bullet; bullet = bullet->next) {
    if (checkEntityCollision(player, &bullet->entity)) {
      
    }
  }
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
