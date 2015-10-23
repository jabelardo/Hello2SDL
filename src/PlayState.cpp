//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "Player.h"
#include "PlayState.h"
#include "TextureId.h"

bool
PlayState::init(GameContext *gameContext) {

//  if (!gameContext->functions.loadTexture("HELICOPTER", "helicopter.png", gameContext->renderer,
//                                          &gameContext->permanentMemory)) {
//    return false;
//  }
  if (!gameContext->functions.loadTexture("HELICOPTER2", "helicopter2.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }

//  SDL_Texture *helicopter = gameContext->functions.getTexture("HELICOPTER");
//  if (!helicopter) {
//    return false;
//  }
  SDL_Texture *helicopter2 = gameContext->functions.getTexture("HELICOPTER2");
  if (!helicopter2) {
    return false;
  }

  tileMap = (TileMap *) reserveMemory(&gameContext->permanentMemory, sizeof(TileMap));

  if (!tileMap->init(gameContext, "/Users/jabelardo/Library/Caches/clion11/cmake/generated/cd64e9d/cd64e9d/Debug/assets/untitled2.tmx")) {
    return false;
  }

//  player = PLACEMENT_NEW(&gameContext->permanentMemory, Player)
//      Player{{{500, 100}, {helicopter, 128, 55, 5, 1, 1}}};

  enemy = (Enemy *) reserveMemory(&gameContext->permanentMemory, sizeof(Entity));
  *enemy = {{{400, 100}, {helicopter2, 128, 55, 5, 1, 1}, {2, .33f}, {0, .33f}}};

  return true;
}

void
PlayState::update(GameContext *gameContext) {
  if (gameContext->userInput->back.endedDown) {
    gameContext->stateChange = PAUSE_PLAY;
    return;
  }
  tileMap->update(gameContext);
  //player->update(gameContext->userInput);
  enemy->update(gameContext->userInput);

  if (checkCollision(&enemy->entity, &tileMap->objectLayer->player->entity)) {
    gameContext->stateChange = GAME_OVER;
  }
}

void
PlayState::render(SDL_Renderer *renderer) {
  tileMap->draw(renderer);
  //player->draw(renderer);
  enemy->draw(renderer);
}

GameStateId
PlayState::getStateId() const {
  return PLAY_STATE;
}
