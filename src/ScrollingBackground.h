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

struct SDL_Renderer;

struct ScrollingBackground {
  V2D position;
  Bitmap bitmap;

  int animSpeed;

  int count;
  int maxCount;

  SDL_Rect srcRect1;
  SDL_Rect srcRect2;

  SDL_Rect destRect1;
  SDL_Rect destRect2;

  int srcRect1Width;
  int srcRect2Width;

  int destRect1Width;
  int destRect2Width;
};

void drawScrollingBackground(ScrollingBackground* scrollingBackground, SDL_Renderer *renderer);

void updateScrollingBackground(ScrollingBackground* scrollingBackground);

#endif //HELLO2SDL_SCROLLINGBACKGROUND_H
