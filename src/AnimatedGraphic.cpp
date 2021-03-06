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
drawAnimatedGraphic(AnimatedGraphic *animatedGraphic, SDL_Renderer *renderer) {
  drawBitmap(renderer, animatedGraphic->x, animatedGraphic->y, &animatedGraphic->bitmap);
}

void
updateAnimatedGraphic(AnimatedGraphic *animatedGraphic) {
  if (animatedGraphic->animationSpeed) {
    animatedGraphic->bitmap.currentColumn = (int) (
        (SDL_GetTicks() / (1000 / animatedGraphic->animationSpeed)) %
        animatedGraphic->bitmap.totalFrames);
  }
}
