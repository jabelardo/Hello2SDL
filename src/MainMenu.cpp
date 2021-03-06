//
// Created by Jose Gutierrez on 10/24/15.
//

#ifdef __APPLE__
#include <SDL2/SDL_stdinc.h>
#else
#include <SDL_stdinc.h>
#endif

#include "MainMenu.h"
#include "AssetsManagement.h"
#include "MemoryPartition.h"
#include "MenuButton.h"
#include "SharedDefinitions.h"

bool
initMainMenu(MainMenu *mainMenu, GameContext *gameContext, SDL_Renderer *renderer, GameMemory* gameMemory) {

  if (!loadTexture("PLAY_BUTTON", "button.png", gameContext->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("EXIT_BUTTON", "exit.png", gameContext->resourcePath, renderer,
                   gameContext, gameMemory)) {
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

  mainMenu->startPlayButton = RESERVE_MEMORY(&gameMemory->permanentMemory, MenuButton);
  *mainMenu->startPlayButton = {100, 100, {playButtonTexture, 400, 100, 3}, START_PLAY};

  mainMenu->exitButton = RESERVE_MEMORY(&gameMemory->permanentMemory, MenuButton);
  *mainMenu->exitButton = {100, 300, {exitButtonTexture, 400, 100, 3}, EXIT_FROM_GAME};

  return true;
}

void
updateMainMenu(MainMenu *mainMenu, GameContext *gameContext, UserInput* userInput) {
  for (int i = 0; i < SDL_arraysize(mainMenu->menuButtons); ++i) {
    MenuButton *menuButton = mainMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext, userInput);
  }
}

void
renderMainMenu(MainMenu *mainMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(mainMenu->menuButtons); ++i) {
    MenuButton *menuButton = mainMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
}