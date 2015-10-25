//
// Created by Jose Gutierrez on 10/24/15.
//

#include "GameOverMenu.h"
#include "TextureStorage.h"

bool
initGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext) {

  if (!loadTexture("GAME_OVER_TEXT", "gameover.png", gameContext->renderer, gameContext)) {
    return false;
  }
  if (!loadTexture("MAIN_BUTTON", "main.png", gameContext->renderer, gameContext)) {
    return false;
  }
  if (!loadTexture("RESTART_BUTTON", "restart.png", gameContext->renderer, gameContext)) {
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
  gameOverMenu->mainMenuButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                              sizeof(MenuButton));
  *gameOverMenu->mainMenuButton = {200, 200, {mainButton, 200, 80, 3, MenuButton::MOUSE_OUT, 1},
                                   mainMenu};

  gameOverMenu->startPlayButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                               sizeof(MenuButton));

  *gameOverMenu->startPlayButton = {200, 300, {restartButton, 200, 80, 3, MenuButton::MOUSE_OUT, 1},
                                    startPlay};

  gameOverMenu->gameOverGraphic = (AnimatedGraphic *) reserveMemory(&gameContext->permanentMemory,
                                                                    sizeof(AnimatedGraphic));
  *gameOverMenu->gameOverGraphic = {200, 100, {gameOverText, 190, 30, 2, 1, 1}, 2};

  return true;
}

void
updateGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(gameOverMenu->menuButtons); ++i) {
    MenuButton *menuButton = gameOverMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext);
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