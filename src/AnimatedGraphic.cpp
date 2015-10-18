//
// Created by Jose Gutierrez on 10/16/15.
//
#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include "AnimatedGraphic.h"
#include "RenderUtils.h"

void
AnimatedGraphic::draw(SDL_Renderer *renderer) {
  drawBitmap(renderer, x, y, &bitmap);
}

void
AnimatedGraphic::update(UserInput *inputHandler) {
  if (animationSpeed) {
    bitmap.currentFrame = (int) ((SDL_GetTicks() / (1000 / animationSpeed)) % bitmap.totalFrames);
  }
}
