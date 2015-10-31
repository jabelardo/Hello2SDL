//
// Created by Jose Gutierrez on 10/24/15.
//

#ifdef __APPLE__
#include <SDL2/SDL_stdinc.h>
#else
#include <SDL_stdinc.h>
#endif

#include "PauseMenu.h"
#include "TextureStorage.h"
#include "MemoryPartition.h"
#include "MenuButton.h"
#include "SharedDefinitions.h"

bool
initPauseMenu(PauseMenu *pauseMenu, GameContext *gameContext, SDL_Renderer *renderer,
              GameMemory* gameMemory, PlatformConfig *platformConfig) {
  if (!loadTexture("MAIN_BUTTON", "main.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }
  if (!loadTexture("RESUME_BUTTON", "resume.png", platformConfig->resourcePath, renderer,
                   gameContext, gameMemory)) {
    return false;
  }

  SDL_Texture *mainButton = getTexture("MAIN_BUTTON", gameContext);
  if (!mainButton) {
    return false;
  }
  SDL_Texture *resumeButton = getTexture("RESUME_BUTTON", gameContext);
  if (!resumeButton) {
    return false;
  }
  pauseMenu->mainMenuButton = RESERVE_MEMORY(&gameMemory->permanentMemory, MenuButton);
  *pauseMenu->mainMenuButton = {200, 100, {mainButton, 200, 80, 3}, MAIN_MENU};

  pauseMenu->resumePlayButton = RESERVE_MEMORY(&gameMemory->permanentMemory, MenuButton);
  *pauseMenu->resumePlayButton = {200, 300, {resumeButton, 200, 80, 3}, RESUME_PLAY};

  return true;
}

void
updatePauseMenu(PauseMenu *pauseMenu, GameContext *gameContext, UserInput* userInput) {
  for (int i = 0; i < SDL_arraysize(pauseMenu->menuButtons); ++i) {
    MenuButton *menuButton = pauseMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext, userInput);
  }
}

void
renderPauseMenu(PauseMenu *pauseMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(pauseMenu->menuButtons); ++i) {
    MenuButton *menuButton = pauseMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
}