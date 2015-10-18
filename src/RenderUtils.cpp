//
// Created by Jose Gutierrez on 10/14/15.
//

#include "RenderUtils.h"

void
drawBitmap(SDL_Renderer *renderer, int x, int y, Bitmap *bitmap, SDL_RendererFlip flip) {
  drawTextureFrame(renderer, bitmap->texture, x, y, bitmap->width, bitmap->height,
                   bitmap->currentRow, bitmap->currentFrame, flip);
}

void
drawTexture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height,
            SDL_RendererFlip flip) {
  auto srcRect = SDL_Rect{};
  auto destRect = SDL_Rect{};
  srcRect.x = 0;
  srcRect.y = 0;
  srcRect.w = destRect.w = width;
  srcRect.h = destRect.h = height;
  destRect.x = x;
  destRect.y = y;
  SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, 0, flip);
}

void
drawTextureFrame(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height,
                 int currentRow, int currentFrame, SDL_RendererFlip flip) {
  auto srcRect = SDL_Rect{};
  auto destRect = SDL_Rect{};
  srcRect.x = width * currentFrame;
  srcRect.y = height * (currentRow - 1);
  srcRect.w = destRect.w = width;
  srcRect.h = destRect.h = height;
  destRect.x = x;
  destRect.y = y;
  SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, 0, flip);
}

