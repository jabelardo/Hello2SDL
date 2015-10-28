//
// Created by Jose Gutierrez on 10/24/15.
//

#include "PlayState.h"
#include "Entity.h"
#include "TextureStorage.h"
#include "MemoryPartition.h"
#include "TileMap.h"
#include "SharedDefinitions.h"

bool
initPlayState(PlayState *playState, GameContext *gameContext, SDL_Renderer *renderer,
              GameMemory* gameMemory, PlatformConfig *platformConfig) {
  if (!loadTexture("HELICOPTER2", "helicopter2.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }

  playState->tileMap = RESERVE_MEMORY(&gameMemory->permanentMemory, TileMap);

  if (!initTileMap(playState->tileMap, "game1.tmx", gameContext, renderer, gameMemory,
                   platformConfig)) {
    return false;
  }

  playState->enemy = RESERVE_MEMORY(&gameMemory->permanentMemory, Entity);

  return true;
}

bool
startGame(PlayState *playState, GameContext *gameContext) {
  SDL_Texture *helicopter2 = getTexture("HELICOPTER2", gameContext);
  if (!helicopter2) {
    return false;
  }
  for (TileLayer *tileLayer = playState->tileMap->tileLayerList; tileLayer;
       tileLayer = tileLayer->next) {
    tileLayer->position = {0, 0};
  }

  playState->tileMap->objectLayer->player->position =
      playState->tileMap->objectLayer->playerInitialPosition;

  *playState->enemy = {ENEMY_TYPE, {400, 100}, {helicopter2, 128, 55, 5, 1, 1}, {2, .33f},
                       {0, .33f}};

  playState->enemy->alpha = 255;
  playState->tileMap->objectLayer->player->alpha = 255;
  playState->tileMap->objectLayer->player->dyingTime = 160;
  playState->tileMap->objectLayer->player->invulnerableTime = 320;
  return true;
}

void
updatePlayState(PlayState *playState, GameContext *gameContext, UserInput* userInput) {
  if (userInput->back.endedDown) {
    gameContext->stateChange = PAUSE_MENU;
    return;
  }
  updateTileMap(playState->tileMap, gameContext, userInput);
  updateEntity(playState->enemy, gameContext, userInput);

  if (checkEntityCollision(playState->enemy, playState->tileMap->objectLayer->player)) {
    if (!playState->tileMap->objectLayer->player->isDying &&
        !playState->tileMap->objectLayer->player->isDead &&
        !playState->tileMap->objectLayer->player->invulnerable) {
      playState->tileMap->objectLayer->player->isDying = true;
    }
    //gameContext->stateChange = GAME_OVER;
  }
}

void
renderPlayState(PlayState *playState, SDL_Renderer *renderer) {
  drawTileMap(playState->tileMap, renderer);
  drawEntity(playState->enemy, renderer);
}