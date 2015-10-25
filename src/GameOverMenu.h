//
// Created by Jose Gutierrez on 10/24/15.
//

#ifndef HELLO2SDL_GAMEOVERMENU_H
#define HELLO2SDL_GAMEOVERMENU_H

#include "MenuButton.h"
#include "AnimatedGraphic.h"

struct GameOverMenu {
  union {
    struct {
      MenuButton *mainMenuButton;
      MenuButton *startPlayButton;
    };
    MenuButton *menuButtons[2];
  };
  AnimatedGraphic *gameOverGraphic;
};

bool initGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext);

void updateGameOverMenu(GameOverMenu *gameOverMenu, GameContext *gameContext);

void renderGameOverMenu(GameOverMenu *gameOverMenu, SDL_Renderer *renderer);

#endif //HELLO2SDL_GAMEOVERMENU_H
