//
// Created by Jose Gutierrez on 10/24/15.
//

#include "PlayState.h"
#include "Entity.h"
#include "TextureStorage.h"

bool
initPlayState(PlayState *playState, GameContext *gameContext) {
  if (!loadTexture("HELICOPTER2", "helicopter2.png", gameContext->renderer, gameContext)) {
    return false;
  }

  playState->tileMap = (TileMap *) reserveMemory(&gameContext->permanentMemory, sizeof(TileMap));

  if (!initTileMap(playState->tileMap, "game1.tmx", gameContext)) {
    return false;
  }

  playState->enemy = (Entity *) reserveMemory(&gameContext->permanentMemory, sizeof(Entity));

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
  return true;
}

void
updatePlayState(PlayState *playState, GameContext *gameContext) {
  if (gameContext->userInput.back.endedDown) {
    gameContext->stateChange = PAUSE_MENU;
    return;
  }
  updateTileMap(playState->tileMap, gameContext);
  updateEntity(playState->enemy, &gameContext->userInput);

  if (checkEntityCollision(playState->enemy, playState->tileMap->objectLayer->player)) {
   // gameContext->stateChange = GAME_OVER;
  }
}

void
renderPlayState(PlayState *playState, SDL_Renderer *renderer) {
  drawTileMap(playState->tileMap, renderer);
  drawEntity(playState->enemy, renderer);
}