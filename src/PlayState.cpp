//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "PlayState.h"
#include "InputHandler.h"
#include "TextureManager.h"
#include "LoaderParams.h"
#include "Game.h"

Game*
PlayState::game = 0;

void
PlayState::update(InputHandler *inputHandler) {
  for (auto &entity : entities) {
    entity->update(inputHandler);
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

  entities.push_back(std::make_unique<Entity>(Entity::PLAYER_TYPE,
                                              LoaderParams{HELICOPTER, 100, 100, 128, 55, 5}));

  return true;
}

bool
PlayState::onExit(TextureManager *textureManager) {
  for (auto &entity : entities) {
    entity->clean();
  }
  entities.clear();
  textureManager->clearFromTextureMap(HELICOPTER);
  return true;
}

int
PlayState::getStateId() const {
  return 1;
}

void
PlayState::setGame(Game *game) {
  PlayState::game = game;
}
