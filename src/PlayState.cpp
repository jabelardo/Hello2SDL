//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "Player.h"
#include "PlayState.h"
#include "Game.h"
#include "TextureId.h"
#include "MemoryPartitionPlacementNew.h"

Game *
PlayState::game = 0;

bool
PlayState::init(Game *game, GameContext *gameContext) {

  this->game = game;

  if (!gameContext->functions.loadTexture(HELICOPTER, "helicopter.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(HELICOPTER2, "helicopter2.png", gameContext->renderer)) {
    return false;
  }

  SDL_Texture *helicopter = gameContext->functions.getTexture(HELICOPTER);
  if (!helicopter) {
    return false;
  }
  SDL_Texture *helicopter2 = gameContext->functions.getTexture(HELICOPTER2);
  if (!helicopter2) {
    return false;
  }

  player = PLACEMENT_NEW(&gameContext->permanentMemory, Player)
      Player{{helicopter, {500, 100}, 128, 55, 5, 1, 1}};

  enemy = PLACEMENT_NEW(&gameContext->permanentMemory, Entity)
      Entity{{helicopter2, {0, 100}, 128, 55, 5, 1, 1}};

  enemy->sprite.velocity = {2, .33f};
  enemy->sprite.acceleration = {0, .33f};

  return true;
}

void
PlayState::update(GameContext* gameContext) {
  assert(game);
  if (gameContext->userInput->back.endedDown) {
    game->pause(gameContext);
    return;
  }
  player->update(gameContext->userInput);
  enemy->update(gameContext->userInput);

  if (Sprite::checkCollision(enemy->sprite, player->sprite)) {
    game->gameOver(gameContext);
  }
}

void
PlayState::render(SDL_Renderer* renderer) {
  player->draw(renderer);
  enemy->draw(renderer);
}

GameStateId
PlayState::getStateId() const {
  return PLAY_STATE;
}
