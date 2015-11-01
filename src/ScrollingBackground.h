//
// Created by Jose Gutierrez on 10/31/15.
//

#ifndef HELLO2SDL_SCROLLINGBACKGROUND_H
#define HELLO2SDL_SCROLLINGBACKGROUND_H

#ifdef __APPLE__
#include <SDL2/SDL_rect.h>
#else
#include <SDL_rect.h>
#endif

#include "V2D.h"
#include "Bitmap.h"

struct GameContext;
struct SDL_Renderer;

struct ScrollingBackground {
  V2D position;
  Bitmap bitmap;
  int animSpeed;
};

void drawScrollingBackground(ScrollingBackground* scrollingBackground, GameContext* gameContext,
                             SDL_Renderer *renderer);

void updateScrollingBackground(ScrollingBackground* scrollingBackground);

#endif //HELLO2SDL_SCROLLINGBACKGROUND_H
