//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "PauseState.h"
#include "MenuButton.h"
#include "TextureManager.h"
#include "LoaderParams.h"
#include "Game.h"

Game*
PauseState::game = 0;

void
PauseState::update(UserInput *inputHandler, SDL_Renderer *renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.update(inputHandler, renderer);
  }
}

void
PauseState::render(TextureManager *textureManager, SDL_Renderer *renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.draw(textureManager, renderer);
  }
}

bool
PauseState::onEnter(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (!textureManager->load(MAIN_BUTTON, "main.png", renderer)) {
    return false;
  }
  if (!textureManager->load(RESUME_BUTTON, "resume.png", renderer)) {
    return false;
  }

  menuButtons.push_back(MenuButton(LoaderParams{MAIN_BUTTON, 200, 100, 200, 80, 3}, pauseToMain));
  menuButtons.push_back(MenuButton(LoaderParams{RESUME_BUTTON, 200, 300, 200, 80, 3}, resumePlay));

  return true;
}

bool
PauseState::onExit(TextureManager *textureManager) {
  assert(game);
  for (auto& menuButton : menuButtons) {
    menuButton.clean();
  }
  menuButtons.clear();
  textureManager->clearFromTextureMap(MAIN_BUTTON);
  textureManager->clearFromTextureMap(RESUME_BUTTON);
  // TODO game->resetInput();

  return true;
}

GameStateId
PauseState::getStateId() const {
  return PAUSE_STATE;
}

void
PauseState::setGame(Game *game) {
  PauseState::game = game;
}

void
PauseState::pauseToMain(SDL_Renderer *renderer) {
  assert(game);
  game->showMenu(renderer);
}

void
PauseState::resumePlay(SDL_Renderer *renderer) {
  assert(game);
  game->resumePlay();
}
