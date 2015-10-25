//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_PAUSEMENU_H
#define HELLO2SDL_PAUSEMENU_H

#include "MenuButton.h"

struct PauseMenu {
  union {
    struct {
      MenuButton *mainMenuButton;
      MenuButton *resumePlayButton;
    };
    MenuButton *menuButtons[2];
  };
};

bool initPauseMenu(PauseMenu *pauseMenu, GameContext *gameContext);

void updatePauseMenu(PauseMenu *pauseMenu, GameContext *gameContext);

void renderPauseMenu(PauseMenu *pauseMenu, SDL_Renderer *renderer);

#endif //HELLO2SDL_PAUSEMENU_H
