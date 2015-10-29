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
  if (!loadTexture("HELICOPTER2", "helicopter2.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }

  playState->tileMap = RESERVE_MEMORY(&gameMemory->permanentMemory, TileMap);

  if (!initTileMap(playState->tileMap, "game1.tmx", gameContext, renderer, gameMemory,
                   platformConfig)) {
    return false;
  }

  playState->enemies = RESERVE_MEMORY(&gameMemory->permanentMemory, EntityNode);
  playState->enemies->next = 0;
  playState->playerBullets = 0;
  playState->enemyBullets = 0;
  playState->freeEntities = 0;

  return true;
}

bool
startGame(PlayState *playState, GameContext *gameContext) {
  SDL_Texture *helicopter2 = getTexture("HELICOPTER2", gameContext);
  if (!helicopter2) {
    return false;
  }
  playState->liveTexture = getTexture("LIVES", gameContext);
  if (!playState->liveTexture) {
    return false;
  }
  playState->bullet1Texture = getTexture("BULLET1", gameContext);
  if (!playState->bullet1Texture) {
    return false;
  }
  for (TileLayer *tileLayer = playState->tileMap->tileLayerList; tileLayer;
       tileLayer = tileLayer->next) {
    tileLayer->position = {0, 0};
  }

  playState->tileMap->objectLayer->player->position =
      playState->tileMap->objectLayer->playerInitialPosition;

  playState->enemies->entity = {ENEMY_TYPE,
                                {400, 100},
                                {helicopter2, 128, 55, 5, 1, 1},
                                {2, .33f},
                                {0, .33f}};

  resetEntity(&playState->enemies->entity);
  resetEntity(playState->tileMap->objectLayer->player);
  playState->tileMap->objectLayer->player->currentLives = 1;

  return true;
}

static void
updateBullets(EntityNode** bullets, PlayState *playState, GameContext *gameContext,
              UserInput *userInput, GameMemory *gameMemory) {
  for (EntityNode **bullet = bullets; *bullet;) {
    if ((*bullet)->entity.position.x < 0 ||
        (*bullet)->entity.position.x > gameContext->gameWidth ||
        (*bullet)->entity.position.y < 0 ||
        (*bullet)->entity.position.y > gameContext->gameHeight ||
        (*bullet)->entity.dyingCounter == 1) {

      (*bullet)->entity.dyingCounter = 0;

      EntityNode* freeEntity = *bullet;

      // NOTE: must modify the pointer itself
      *bullet = (*bullet)->next;

      freeEntity->next = playState->freeEntities;
      playState->freeEntities = freeEntity;
      assert((*bullet == 0) || (*bullet != (*bullet)->next));

    } else {
      updateEntity(&(*bullet)->entity, gameContext, userInput, playState, gameMemory);
      bullet = &(*bullet)->next;
      assert((*bullet == 0) || (*bullet != (*bullet)->next));
    }
  }
}

void
updatePlayState(PlayState *playState, GameContext *gameContext, UserInput *userInput,
                GameMemory *gameMemory) {
  if (userInput->back.endedDown) {
    gameContext->stateChange = PAUSE_MENU;
    return;
  }
  updateTileMap(playState->tileMap, gameContext, userInput, playState, gameMemory);

  updateEntity(&playState->enemies->entity, gameContext, userInput, playState, gameMemory);

  updateBullets(&playState->playerBullets, playState, gameContext, userInput,gameMemory);

  updateBullets(&playState->enemyBullets, playState, gameContext, userInput,gameMemory);
  
  if (checkEntityCollision(&playState->enemies->entity, playState->tileMap->objectLayer->player)) {
    if (!playState->tileMap->objectLayer->player->dyingCounter &&
        !playState->tileMap->objectLayer->player->invulnerableCounter) {
//      playState->tileMap->objectLayer->player->dyingCounter =
//          playState->tileMap->objectLayer->player->dyingTime;
    }
  }
}

void
renderPlayState(PlayState *playState, SDL_Renderer *renderer) {
  drawTileMap(playState->tileMap, renderer);
  drawEntity(&playState->enemies->entity, renderer);

  for (int i = 0; i < playState->tileMap->objectLayer->player->currentLives; ++i) {
    drawTextureFrame(renderer, playState->liveTexture, i * 30, 0, 32, 30, 1, 0);
  }

  int i = 0;
  for (EntityNode *bullet = playState->playerBullets; bullet; bullet = bullet->next) {
    ++i;
    if (i > 1) {
      int a = 0;
    }
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

  bullet->next = (bullet != playState->playerBullets) ? playState->playerBullets : 0;

  playState->playerBullets = bullet;

  bullet->entity = {PLAYER_BULLET_TYPE,
                    position,
                    {playState->bullet1Texture, 11, 11, 1, 0, 1},
                    velocity};

  assert(bullet != bullet->next);
}

void
addEnemyBullet(int x, int y, int width, int height, const char* textureID, int numFrames,
               V2D heading) {
//  EnemyBullet* pEnemyBullet = new EnemyBullet();
//  pEnemyBullet->load(std::unique_ptr<LoaderParams>(new
//                                                       LoaderParams(x, y, width, height, textureID, numFrames)),
//                     heading);
//  m_enemyBullets.push_back(pEnemyBullet);
}
