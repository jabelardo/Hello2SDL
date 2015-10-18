//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "GameOverState.h"
#include "TextureId.h"
#include "MemoryPartitionPlacementNew.h"

bool
GameOverState::init(GameContext *gameContext) {

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
  if (!mainButton) {
    return false;
  }
  SDL_Texture *restartButton = gameContext->functions.getTexture(RESTART_BUTTON);
  if (!restartButton) {
    return false;
  }
  SDL_Texture *gameOverText = gameContext->functions.getTexture(GAME_OVER_TEXT);
  if (!gameOverText) {
    return false;
  }
  menuButtons[0] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
      MenuButton({mainButton, {200, 200}, 200, 80, 3, 1, 1}, gameOverToMain);

  menuButtons[1] = PLACEMENT_NEW(&gameContext->permanentMemory, MenuButton)
      MenuButton({restartButton, {200, 300}, 200, 80, 3, 1, 1}, restartPlay);

  gameOverGraphic = PLACEMENT_NEW(&gameContext->permanentMemory, AnimatedGraphic)
      AnimatedGraphic{{gameOverText, {200, 100}, 190, 30, 2, 1, 1}, 2};

  return true;
}

void
GameOverState::update(GameContext* gameContext) {
  for (auto &menuButton : menuButtons) {
    menuButton->update(gameContext);
  }
  gameOverGraphic->update(gameContext->userInput);
}

void
GameOverState::render(SDL_Renderer* renderer) {
  for (auto &menuButton : menuButtons) {
    menuButton->draw(renderer);
  }
  gameOverGraphic->draw(renderer);
}

GameStateId
GameOverState::getStateId() const {
  return GAME_OVER_STATE;
}

void
GameOverState::gameOverToMain(GameContext* gameContext) {
  gameContext->stateChange = SHOW_MAIN_MENU;
}

void
GameOverState::restartPlay(GameContext* gameContext) {
  gameContext->stateChange = RESTART_PLAY;
}
