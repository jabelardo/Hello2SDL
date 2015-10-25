//
// Created by Jose Gutierrez on 10/24/15.
//

#include "GameOverMenu.h"
#include "TextureStorage.h"
#include "MemoryPartition.h"
#include "MenuButton.h"
#include "AnimatedGraphic.h"
#include "SharedDefinitions.h"

bool
initGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext, SDL_Renderer *renderer,
                 GameMemory* gameMemory, PlatformConfig *platformConfig) {

  if (!loadTexture("GAME_OVER_TEXT", "gameover.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("MAIN_BUTTON", "main.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("RESTART_BUTTON", "restart.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  SDL_Texture *mainButton = getTexture("MAIN_BUTTON", gameContext);
  if (!mainButton) {
    return false;
  }
  SDL_Texture *restartButton = getTexture("RESTART_BUTTON", gameContext);
  if (!restartButton) {
    return false;
  }
  SDL_Texture *gameOverText = getTexture("GAME_OVER_TEXT", gameContext);
  if (!gameOverText) {
    return false;
  }
  gameOverMenu->mainMenuButton = RESERVE_MEMORY(&gameMemory->permanentMemory, MenuButton);
  *gameOverMenu->mainMenuButton = {200, 200, {mainButton, 200, 80, 3, MenuButton::MOUSE_OUT, 1},
                                   MAIN_MENU};

  gameOverMenu->startPlayButton = RESERVE_MEMORY(&gameMemory->permanentMemory, MenuButton);

  *gameOverMenu->startPlayButton = {200, 300, {restartButton, 200, 80, 3, MenuButton::MOUSE_OUT, 1},
                                    START_PLAY};

  gameOverMenu->gameOverGraphic = RESERVE_MEMORY(&gameMemory->permanentMemory, AnimatedGraphic);
  *gameOverMenu->gameOverGraphic = {200, 100, {gameOverText, 190, 30, 2, 1, 1}, 2};

  return true;
}

void
updateGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext, UserInput* userInput) {
  for (int i = 0; i < SDL_arraysize(gameOverMenu->menuButtons); ++i) {
    MenuButton *menuButton = gameOverMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext, userInput);
  }
  updateAnimatedGraphic(gameOverMenu->gameOverGraphic);
}

void
renderGameOverMenu(GameOverMenu *gameOverMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(gameOverMenu->menuButtons); ++i) {
    MenuButton *menuButton = gameOverMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
  drawAnimatedGraphic(gameOverMenu->gameOverGraphic, renderer);
}