//
// Created by Jose Gutierrez on 10/31/15.
//

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "ScrollingBackground.h"
#include "RenderUtils.h"
#include "Game.h"

void updateScrollingBackground(ScrollingBackground *scrollingBackground, GameContext *gameContext) {
  scrollingBackground->position.x += scrollingBackground->animSpeed * gameContext->scrollSpeed;
  if (scrollingBackground->position.x >= scrollingBackground->bitmap.width ||
      scrollingBackground->position.x <= -scrollingBackground->bitmap.width) {
    scrollingBackground->position.x = 0;
  }
}

void drawScrollingBackground(ScrollingBackground* scrollingBackground, GameContext* gameContext,
                             SDL_Renderer *renderer) {
  if (scrollingBackground->position.x >= 0) {

    SDL_Rect srcMainRect;
    srcMainRect.x = (int) scrollingBackground->position.x;
    srcMainRect.y = (int) scrollingBackground->position.y;

    SDL_Rect destMainRect;
    destMainRect.x = 0;
    destMainRect.y = 0;

    srcMainRect.h = destMainRect.h = gameContext->gameHeight;

    if (scrollingBackground->position.x < scrollingBackground->bitmap.width - gameContext->gameWidth) {
      srcMainRect.w = destMainRect.w = gameContext->gameWidth;

    } else {

      int mainRectWidth = scrollingBackground->bitmap.width - (int) scrollingBackground->position.x;

      srcMainRect.w = destMainRect.w = mainRectWidth;

      SDL_Rect srcAuxRect;
      srcAuxRect.x = 0;
      srcAuxRect.y = (int) scrollingBackground->position.y;

      SDL_Rect destAuxRect;
      destAuxRect.x = mainRectWidth;
      destAuxRect.y = 0;

      srcAuxRect.w = destAuxRect.w = gameContext->gameWidth - destAuxRect.x;
      srcAuxRect.h = destAuxRect.h = gameContext->gameHeight;

      SDL_RenderCopy(renderer, scrollingBackground->bitmap.texture, &srcAuxRect, &destAuxRect);
    }

    SDL_RenderCopy(renderer, scrollingBackground->bitmap.texture, &srcMainRect, &destMainRect);

  } else {

    SDL_Rect srcMainRect;
    srcMainRect.y = (int) scrollingBackground->position.y;

    SDL_Rect destMainRect;
    destMainRect.y = 0;

    srcMainRect.h = destMainRect.h = gameContext->gameHeight;

    if (gameContext->gameWidth + scrollingBackground->position.x < 0) {

      srcMainRect.x = scrollingBackground->bitmap.width + (int) scrollingBackground->position.x;
      destMainRect.x = 0;
      srcMainRect.w = destMainRect.w = gameContext->gameWidth;

    } else {

      srcMainRect.x = 0;
      destMainRect.x = -1 * (int) scrollingBackground->position.x;
      srcMainRect.w = destMainRect.w = gameContext->gameWidth - destMainRect.x;

      SDL_Rect srcAuxRect;
      srcAuxRect.x = scrollingBackground->bitmap.width - destMainRect.x;
      srcAuxRect.y = (int) scrollingBackground->position.y;

      SDL_Rect destAuxRect;
      destAuxRect.x = 0;
      destAuxRect.y = 0;

      srcAuxRect.w = destAuxRect.w = gameContext->gameWidth - srcMainRect.w;
      srcAuxRect.h = destAuxRect.h = gameContext->gameHeight;

      SDL_RenderCopy(renderer, scrollingBackground->bitmap.texture, &srcAuxRect, &destAuxRect);
    }

    SDL_RenderCopy(renderer, scrollingBackground->bitmap.texture, &srcMainRect, &destMainRect);
  }
}
