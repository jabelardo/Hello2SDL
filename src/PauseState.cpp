//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "PauseState.h"
#include "Game.h"
#include "TextureId.h"

Game*
PauseState::game = 0;

void
PauseState::update(GameContext* gameContext) {
  for (auto &menuButton : menuButtons) {
    menuButton.update(gameContext);
  }
}

void
PauseState::render(SDL_Renderer* renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.draw(renderer);
  }
}

bool
PauseState::onEnter(GameContext* gameContext) {
  if (!gameContext->functions.loadTexture(MAIN_BUTTON, "main.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(RESUME_BUTTON, "resume.png", gameContext->renderer)) {
    return false;
  }

  SDL_Texture *mainButton = gameContext->functions.getTexture(MAIN_BUTTON);
  SDL_Texture *resumeButton = gameContext->functions.getTexture(RESUME_BUTTON);

  menuButtons.push_back(MenuButton(Sprite{mainButton, {200, 100}, 200, 80, 3, 1, 1}, pauseToMain));
  menuButtons.push_back(MenuButton(Sprite{resumeButton, {200, 300}, 200, 80, 3, 1, 1}, resumePlay));

  return true;
}

bool
PauseState::onExit(GameContext* gameContext) {
  assert(game);
  menuButtons.clear();
  gameContext->functions.unloadTexture(MAIN_BUTTON);
  gameContext->functions.unloadTexture(RESUME_BUTTON);

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
PauseState::pauseToMain(GameContext* gameContext) {
  assert(game);
  game->showMenu(gameContext);
}

void
PauseState::resumePlay(GameContext* gameContext) {
  assert(game);
  game->resumePlay(gameContext);
}
