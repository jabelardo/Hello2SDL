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

#include "Sprite.h"
#include "UserInput.h"
#include "GameContext.h"

struct MenuButton {

  typedef void(CallbackFunc)(GameContext*);

  Sprite sprite;
  CallbackFunc* callback;
  bool buttonReleased;

  MenuButton(const Sprite& sprite, CallbackFunc* callback);

  void draw(SDL_Renderer* renderer);

  void update(GameContext *gameContext);
};


#endif //HELLO2SDL_MENUBUTTON_H
