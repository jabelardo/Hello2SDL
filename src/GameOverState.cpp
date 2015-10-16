//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "GameOverState.h"
#include "TextureManager.h"
#include "LoaderParams.h"
#include "Game.h"
#include "GameStateId.h"

Game *
GameOverState::game = 0;

void
GameOverState::update(InputHandler *inputHandler) {
  for (auto &menuButton : menuButtons) {
    menuButton.update(inputHandler);
  }
  if (gameOverGraphic) {
    gameOverGraphic->update(inputHandler);
  }
}

void
GameOverState::render(TextureManager *textureManager, SDL_Renderer *renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.draw(textureManager, renderer);
  }
  if (gameOverGraphic) {
    gameOverGraphic->draw(textureManager, renderer);
  }
}

bool
GameOverState::onEnter(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (!textureManager->load(GAME_OVER_TEXT, "gameover.png", renderer)) {
    return false;
  }
  if (!textureManager->load(MAIN_BUTTON, "main.png", renderer)) {
    return false;
  }
  if (!textureManager->load(RESTART_BUTTON, "restart.png", renderer)) {
    return false;
  }

  menuButtons.push_back(MenuButton(LoaderParams{MAIN_BUTTON, 200, 200, 200, 80, 3}, gameOverToMain));

  menuButtons.push_back(MenuButton(LoaderParams{RESTART_BUTTON, 200, 300, 200, 80, 3}, restartPlay));

  gameOverGraphic = std::make_unique<AnimatedGraphic>(LoaderParams{GAME_OVER_TEXT, 200, 100, 190, 30, 2}, 2);

  return true;
}

bool
GameOverState::onExit(TextureManager *textureManager) {
  for (auto &menuButton : menuButtons) {
    menuButton.clean();
  }
  if (gameOverGraphic) {
    gameOverGraphic->clean();
  }
  menuButtons.clear();
  textureManager->clearFromTextureMap(MAIN_BUTTON);
  textureManager->clearFromTextureMap(RESTART_BUTTON);
  textureManager->clearFromTextureMap(GAME_OVER_TEXT);
  return true;
}

GameStateId
GameOverState::getStateId() const {
  return GAME_OVER_STATE;
}

void
GameOverState::setGame(Game *game) {
  GameOverState::game = game;
}

void
GameOverState::gameOverToMain() {
  assert(game);
  game->showMenu();
}

void
GameOverState::restartPlay() {
  assert(game);
  game->play();
}
