//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "Player.h"
#include "PlayState.h"
#include "TextureManager.h"
#include "LoaderParams.h"
#include "Game.h"
#include "Sprite.h"
#include "Entity.h"

Game *
PlayState::game = 0;

void
PlayState::update(UserInput *userInput, SDL_Renderer *renderer) {
  assert(game);
  if (userInput->back.endedDown) {
    game->pause(renderer);
  }
  player->update(userInput);
  for (auto &entity : entities) {
    entity->update(userInput);
  }
  if (Sprite::checkCollision(*entities[0], *player)) {
    game->gameOver(renderer);
  }
}

void
PlayState::render(TextureManager *textureManager, SDL_Renderer *renderer) {
  player->draw(textureManager, renderer);
  for (auto &entity : entities) {
    entity->draw(textureManager, renderer);
  }
}

bool
PlayState::onEnter(TextureManager *textureManager, SDL_Renderer *renderer) {

  if (!textureManager->load(HELICOPTER, "helicopter.png", renderer)) {
    return false;
  }
  if (!textureManager->load(HELICOPTER2, "helicopter2.png", renderer)) {
    return false;
  }

  player = std::make_unique<Player>(LoaderParams{HELICOPTER, 500, 100, 128, 55, 5});

  entities.push_back(std::make_unique<Entity>(LoaderParams{HELICOPTER2, 0, 100, 128, 55, 5}));
  entities.back()->velocity = {2, .33f};
  entities.back()->acceleration = {0, .33f};

  return true;
}

bool
PlayState::onExit(TextureManager *textureManager) {
  entities.clear();
  textureManager->clearFromTextureMap(HELICOPTER);
  textureManager->clearFromTextureMap(HELICOPTER2);
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
