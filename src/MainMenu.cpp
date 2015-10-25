//
// Created by Jose Gutierrez on 10/24/15.
//

#include "MainMenu.h"
#include "TextureStorage.h"

bool
initMainMenu(MainMenu *mainMenu, GameContext *gameContext) {

  if (!loadTexture("PLAY_BUTTON", "button.png", gameContext->renderer, gameContext)) {
    return false;
  }
  if (!loadTexture("EXIT_BUTTON", "exit.png", gameContext->renderer, gameContext)) {
    return false;
  }

  SDL_Texture *playButtonTexture = getTexture("PLAY_BUTTON", gameContext);
  if (!playButtonTexture) {
    return false;
  }
  SDL_Texture *exitButtonTexture = getTexture("EXIT_BUTTON", gameContext);
  if (!exitButtonTexture) {
    return false;
  }

  mainMenu->startPlayButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                           sizeof(MenuButton));
  *mainMenu->startPlayButton = {100, 100, {playButtonTexture, 400, 100, 3, 1, 1}, startPlay};

  mainMenu->exitButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                      sizeof(MenuButton));
  *mainMenu->exitButton = {100, 300, {exitButtonTexture, 400, 100, 3, 1, 1}, exitFromGame};

  return true;
}

void
updateMainMenu(MainMenu *mainMenu, GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(mainMenu->menuButtons); ++i) {
    MenuButton *menuButton = mainMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext);
  }
}

void
renderMainMenu(MainMenu *mainMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(mainMenu->menuButtons); ++i) {
    MenuButton *menuButton = mainMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
}