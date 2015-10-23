//
// Created by Jose Gutierrez on 10/16/15.
//

#include <assert.h>
#include "PauseState.h"
#include "TextureId.h"

bool
PauseState::init(GameContext *gameContext) {

  if (!gameContext->functions.loadTexture("MAIN_BUTTON", "main.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }
  if (!gameContext->functions.loadTexture("RESUME_BUTTON", "resume.png", gameContext->renderer,
                                          &gameContext->permanentMemory)) {
    return false;
  }

  SDL_Texture *mainButton = gameContext->functions.getTexture("MAIN_BUTTON");
  if (!mainButton) {
    return false;
  }
  SDL_Texture *resumeButton = gameContext->functions.getTexture("RESUME_BUTTON");
  if (!resumeButton) {
    return false;
  }
  menuButtons[0] = (MenuButton *) reserveMemory(&gameContext->permanentMemory, sizeof(MenuButton));
  *menuButtons[0] = {200, 100, {mainButton, 200, 80, 3, 1, 1}, pauseToMain};

  menuButtons[1] = (MenuButton *) reserveMemory(&gameContext->permanentMemory, sizeof(MenuButton));
  *menuButtons[1] = {200, 300, {resumeButton, 200, 80, 3, 1, 1}, resumePlay};

  return true;
}

void
PauseState::update(GameContext *gameContext) {
  for (int i = 0; i < SDL_arraysize(menuButtons); ++i) {
    MenuButton *menuButton = menuButtons[i];
    menuButton->update(gameContext);
  }
}

void
PauseState::render(SDL_Renderer *renderer) {
  for (int i = 0; i < SDL_arraysize(menuButtons); ++i) {
    MenuButton *menuButton = menuButtons[i];
    menuButton->draw(renderer);
  }
}

GameStateId
PauseState::getStateId() const {
  return PAUSE_STATE;
}

void
PauseState::pauseToMain(GameContext *gameContext) {
  gameContext->stateChange = SHOW_MAIN_MENU;
}

void
PauseState::resumePlay(GameContext *gameContext) {
  gameContext->stateChange = RESUME_PLAY;
}
