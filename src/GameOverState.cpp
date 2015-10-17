//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "GameOverState.h"
#include "Game.h"
#include "TextureId.h"

Game *
GameOverState::game = 0;

void
GameOverState::update(GameContext* gameContext) {
  for (auto &menuButton : menuButtons) {
    menuButton.update(gameContext);
  }

    gameOverGraphic.update(gameContext->userInput);

}

void
GameOverState::render(SDL_Renderer* renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton.draw(renderer);
  }

    gameOverGraphic.draw(renderer);

}

bool
GameOverState::onEnter(GameContext* gameContext) {
  if (!gameContext->functions.loadTexture(GAME_OVER_TEXT, "gameover.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(MAIN_BUTTON, "main.png", gameContext->renderer)) {
    return false;
  }
  if (!gameContext->functions.loadTexture(RESTART_BUTTON, "restart.png", gameContext->renderer)) {
    return false;
  }

  SDL_Texture *mainButton = gameContext->functions.getTexture(MAIN_BUTTON);
  SDL_Texture *restartButton = gameContext->functions.getTexture(RESTART_BUTTON);
  SDL_Texture *gameOverText = gameContext->functions.getTexture(GAME_OVER_TEXT);

  menuButtons.push_back(MenuButton({mainButton, {200, 200}, 200, 80, 3, 1, 1}, gameOverToMain));

  menuButtons.push_back(MenuButton({restartButton, {200, 300}, 200, 80, 3, 1, 1}, restartPlay));

  gameOverGraphic = AnimatedGraphic{{gameOverText, {200, 100}, 190, 30, 2, 1, 1}, 2};

  return true;
}

bool
GameOverState::onExit(GameContext* gameContext) {
  menuButtons.clear();
  gameContext->functions.unloadTexture(MAIN_BUTTON);
  gameContext->functions.unloadTexture(RESTART_BUTTON);
  gameContext->functions.unloadTexture(GAME_OVER_TEXT);
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
GameOverState::gameOverToMain(GameContext* gameContext) {
  assert(game);
  game->showMenu(gameContext);
}

void
GameOverState::restartPlay(GameContext* gameContext) {
  assert(game);
  game->play(gameContext);
}
