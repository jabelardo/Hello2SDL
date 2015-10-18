//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUBUTTON_H
#define HELLO2SDL_MENUBUTTON_H

#ifdef __APPLE__

#include <SDL2/SDL_render.h>

#else
#include <SDL_render.h>
#endif

#include "Bitmap.h"
#include "UserInput.h"
#include "GameContext.h"

struct MenuButton {

  enum MouseButtonFrame {
    MOUSE_OUT = 0,
    MOUSE_OVER,
    CLICKED
  };

  int x;
  int y;
  Bitmap bitmap;
  GameContextCallbackFunc *callback;
  bool buttonReleased;

  void draw(SDL_Renderer *renderer);

  void update(GameContext *gameContext);
};


#endif //HELLO2SDL_MENUBUTTON_H
