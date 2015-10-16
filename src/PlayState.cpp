//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "PlayState.h"
#include "InputHandler.h"
#include "TextureManager.h"
#include "LoaderParams.h"
#include "Game.h"
#include "Sprite.h"
#include "Entity.h"

Game *
    PlayState::game = 0;

void
PlayState::update(InputHandler *inputHandler) {
  assert(game);
  if (inputHandler->isKeyDown(SDL_SCANCODE_ESCAPE)) {
    game->pause();
  }
  for (auto &entity : entities) {
    entity->update(inputHandler);
  }
  if (Sprite::checkCollision(entities[0]->getSprite(), entities[1]->getSprite())) {
    game->gameOver();
  }
}

void
PlayState::render(TextureManager *textureManager, SDL_Renderer *renderer) {
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

  entities.push_back(std::make_unique<Entity>(Entity::PLAYER_TYPE,
                                              LoaderParams{HELICOPTER, 500, 100, 128, 55, 5}));

  entities.push_back(std::make_unique<Entity>(Entity::ENEMY_TYPE,
                                              LoaderParams{HELICOPTER2, 0, 100, 128, 55, 5}));

  return true;
}

bool
PlayState::onExit(TextureManager *textureManager) {
  for (auto &entity : entities) {
    entity->clean();
  }
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
