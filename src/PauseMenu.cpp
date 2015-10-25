//
// Created by Jose Gutierrez on 10/24/15.
//

#include "PauseMenu.h"
#include "TextureStorage.h"

bool
initPauseMenu(PauseMenu *pauseMenu, GameContext *gameContext) {
  if (!loadTexture("MAIN_BUTTON", "main.png", gameContext->renderer, gameContext)) {
    return false;
  }
  if (!loadTexture("RESUME_BUTTON", "resume.png", gameContext->renderer, gameContext)) {
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
  pauseMenu->mainMenuButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                           sizeof(MenuButton));
  *pauseMenu->mainMenuButton = {200, 100, {mainButton, 200, 80, 3, 1, 1}, mainMenu};

  pauseMenu->resumePlayButton = (MenuButton *) reserveMemory(&gameContext->permanentMemory,
                                                             sizeof(MenuButton));
  *pauseMenu->resumePlayButton = {200, 300, {resumeButton, 200, 80, 3, 1, 1}, resumePlay};

  return true;
}

void
updatePauseMenu(PauseMenu *pauseMenu, GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(pauseMenu->menuButtons); ++i) {
    MenuButton *menuButton = pauseMenu->menuButtons[i];
    updateMenuButton(menuButton, gameContext);
  }
}

void
renderPauseMenu(PauseMenu *pauseMenu, SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(pauseMenu->menuButtons); ++i) {
    MenuButton *menuButton = pauseMenu->menuButtons[i];
    drawMenuButton(menuButton, renderer);
  }
}