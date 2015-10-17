//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "Player.h"
#include "PlayState.h"
#include "Game.h"
#include "TextureId.h"

Game *
PlayState::game = 0;

void
PlayState::update(GameContext* gameContext) {
  assert(game);
  if (gameContext->userInput->back.endedDown) {
    game->pause(gameContext);
    return;
  }
  player.update(gameContext->userInput);
  for (auto &entity : entities) {
    entity.update(gameContext->userInput);
  }
  if (Sprite::checkCollision(entities[0].sprite, player.sprite)) {
    game->gameOver(gameContext);
  }
}

void
PlayState::render(SDL_Renderer* renderer) {
  player.draw(renderer);
  for (auto &entity : entities) {
    entity.draw(renderer);
  }
}

bool
PlayState::onEnter(GameContext* gameContext) {

  if (!gameContext->functions.loadTexture(HELICOPTER, "helicopter.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(HELICOPTER2, "helicopter2.png", gameContext->renderer)) {
    return false;
  }

  SDL_Texture *helicopter = gameContext->functions.getTexture(HELICOPTER);
  SDL_Texture *helicopter2 = gameContext->functions.getTexture(HELICOPTER2);

  player = Player{{helicopter, {500, 100}, 128, 55, 5, 1, 1}};

  entities.push_back(Entity{{helicopter2, {0, 100}, 128, 55, 5, 1, 1}});
  entities.back().sprite.velocity = {2, .33f};
  entities.back().sprite.acceleration = {0, .33f};

  return true;
}

bool
PlayState::onExit(GameContext* gameContext) {
  entities.clear();
  gameContext->functions.unloadTexture(HELICOPTER);
  gameContext->functions.unloadTexture(HELICOPTER2);
  return true;
}

GameStateId
PlayState::getStateId() const {
  return PLAY_STATE;
}

void
PlayState::setGame(Game *game) {
  PlayState::game = game;
}
