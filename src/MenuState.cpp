//
// Created by Jose Gutierrez on 10/15/15.
//

#include <assert.h>
#include "MenuState.h"
#include "Game.h"
#include "TextureId.h"

Game*
MenuState::game = 0;

void
MenuState::update(GameContext* gameContext) {
  for (auto &menuButton : menuButtons) {
    menuButton.update(gameContext);
  }
}

void
MenuState::render(SDL_Renderer* renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.draw(renderer);
  }
}

bool
MenuState::onEnter(GameContext* gameContext) {

  if (!gameContext->functions.loadTexture(PLAY_BUTTON, "button.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(EXIT_BUTTON, "exit.png", gameContext->renderer)) {
    return false;
  }

  SDL_Texture *playButton = gameContext->functions.getTexture(PLAY_BUTTON);
  SDL_Texture *exitButton = gameContext->functions.getTexture(EXIT_BUTTON);

  menuButtons.push_back(MenuButton({playButton, {100, 100}, 400, 100, 3, 1, 1}, menuToPlay));
  menuButtons.push_back(MenuButton({exitButton, {100, 300}, 400, 100, 3, 1, 1}, exitFromMenu));

  return true;
}

bool
MenuState::onExit(GameContext* gameContext) {
  menuButtons.clear();
  gameContext->functions.unloadTexture(PLAY_BUTTON);
  gameContext->functions.unloadTexture(EXIT_BUTTON);
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
MenuState::menuToPlay(GameContext* gameContext) {
  assert(game);
  game->play(gameContext);

}

void
MenuState::exitFromMenu(GameContext* gameContext) {
  assert(game);
  game->quit();
}
