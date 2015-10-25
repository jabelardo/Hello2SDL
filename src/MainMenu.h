//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_MAINMENU_H
#define HELLO2SDL_MAINMENU_H

#include "MenuButton.h"

struct MainMenu {
  union {
    struct {
      MenuButton *startPlayButton;
      MenuButton *exitButton;
    };
    MenuButton *menuButtons[2];
  };
};

bool initMainMenu(MainMenu *mainMenu, GameContext *gameContext);

void updateMainMenu(MainMenu *mainMenu, GameContext *gameContext);

void renderMainMenu(MainMenu *mainMenu, SDL_Renderer *renderer);

#endif //HELLO2SDL_MAINMENU_H
