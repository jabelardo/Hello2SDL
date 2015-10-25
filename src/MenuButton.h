//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUBUTTON_H
#define HELLO2SDL_MENUBUTTON_H

#include "Bitmap.h"
#include "Game.h"

struct UserInput;

struct MenuButton {

  enum MouseButtonFrame {
    MOUSE_OUT = 0,
    MOUSE_OVER,
    CLICKED
  };

  int x;
  int y;
  Bitmap bitmap;
  GameStateChange stateChange;
  uint32_t clickedTime;
};

void drawMenuButton(MenuButton* menuButton, SDL_Renderer *renderer);

void updateMenuButton(MenuButton* menuButton, GameContext *gameContext, UserInput* userInput);


#endif //HELLO2SDL_MENUBUTTON_H
