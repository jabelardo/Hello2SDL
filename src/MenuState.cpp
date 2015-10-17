//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "MenuState.h"
#include "MenuButton.h"
#include "TextureManager.h"
#include "LoaderParams.h"
#include "Game.h"

Game*
MenuState::game = 0;

void
MenuState::update(UserInput *inputHandler, SDL_Renderer *renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.update(inputHandler, renderer);
  }
}

void
MenuState::render(TextureManager *textureManager, SDL_Renderer *renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.draw(textureManager, renderer);
  }
}

bool
MenuState::onEnter(TextureManager *textureManager, SDL_Renderer *renderer) {

  if (!textureManager->load(PLAY_BUTTON, "button.png", renderer)) {
    return false;
  }
  if (!textureManager->load(EXIT_BUTTON, "exit.png", renderer)) {
    return false;
  }

  menuButtons.push_back(MenuButton(LoaderParams{PLAY_BUTTON, 100, 100, 400, 100, 3}, menuToPlay));
  menuButtons.push_back(MenuButton(LoaderParams{EXIT_BUTTON, 100, 300, 400, 100, 3}, exitFromMenu));

  return true;
}

bool
MenuState::onExit(TextureManager *textureManager) {
  for (auto &menuButton : menuButtons) {
    menuButton.clean();
  }
  menuButtons.clear();
  textureManager->clearFromTextureMap(PLAY_BUTTON);
  textureManager->clearFromTextureMap(EXIT_BUTTON);
  return true;
}

GameStateId
MenuState::getStateId() const {
  return MENU_STATE;
}

void
MenuState::setGame(Game *game) {
  MenuState::game = game;
}

void
MenuState::menuToPlay(SDL_Renderer *renderer) {
  assert(game);
  game->play(renderer);

}

void
MenuState::exitFromMenu(SDL_Renderer *renderer) {
  assert(game);
  game->quit();
}
