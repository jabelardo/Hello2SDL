//
// Created by Jose Gutierrez on 10/31/15.
//

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "ScrollingBackground.h"

void updateScrollingBackground(ScrollingBackground *scrollingBackground) {
  if (scrollingBackground->count == scrollingBackground->maxCount) {
    // make first rectangle smaller
    scrollingBackground->srcRect1.x += scrollingBackground->animSpeed;
    scrollingBackground->srcRect1.w -= scrollingBackground->animSpeed;
    scrollingBackground->destRect1.w -= scrollingBackground->animSpeed;

    // make second rectangle bigger
    scrollingBackground->srcRect2.w += scrollingBackground->animSpeed;
    scrollingBackground->destRect2.w += scrollingBackground->animSpeed;
    scrollingBackground->destRect2.x -= scrollingBackground->animSpeed;

    // reset and start again
    if (scrollingBackground->destRect2.w >= scrollingBackground->bitmap.width) {
      scrollingBackground->srcRect1.x = 0;
      scrollingBackground->destRect1.x = scrollingBackground->position.x;
      scrollingBackground->srcRect1.y = 0;
      scrollingBackground->destRect1.y = scrollingBackground->position.y;

      scrollingBackground->srcRect1.w = scrollingBackground->destRect1.w =
        scrollingBackground->srcRect2Width = scrollingBackground->destRect1Width =
          scrollingBackground->bitmap.width;

      scrollingBackground->srcRect1.h = scrollingBackground->destRect1.h =
          scrollingBackground->bitmap.height;

      scrollingBackground->srcRect2.x = 0;
      scrollingBackground->destRect2.x =
          scrollingBackground->position.x + scrollingBackground->bitmap.width;
      scrollingBackground->srcRect2.y = 0;
      scrollingBackground->destRect2.y = scrollingBackground->position.y;

      scrollingBackground->srcRect2.w = scrollingBackground->destRect2.w =
        scrollingBackground->srcRect2Width = scrollingBackground->destRect2Width = 0;

      scrollingBackground->srcRect2.h = scrollingBackground->destRect2.h =
          scrollingBackground->bitmap.height;
    }
    scrollingBackground->count = 0;
  }

  ++scrollingBackground->count;
}

void drawScrollingBackground(ScrollingBackground *scrollingBackground, SDL_Renderer *renderer) {
  SDL_RenderCopyEx(renderer, scrollingBackground->bitmap.texture, &scrollingBackground->srcRect1,
                   &scrollingBackground->destRect1, 0, 0, SDL_FLIP_NONE);

  SDL_RenderCopyEx(renderer, scrollingBackground->bitmap.texture, &scrollingBackground->srcRect2,
                   &scrollingBackground->destRect2, 0, 0, SDL_FLIP_NONE);
}
