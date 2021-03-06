//
// Created by Jose Gutierrez on 10/14/15.
//

#include "RenderUtils.h"
#include "Bitmap.h"

void
drawTile(SDL_Renderer *renderer, int x, int y, Bitmap *bitmap) {
  SDL_Rect srcRect;
  SDL_Rect destRect;
  srcRect.x = bitmap->margin + (bitmap->spacing + bitmap->width) * bitmap->currentColumn;
  srcRect.y = bitmap->margin + (bitmap->spacing + bitmap->height) * bitmap->currentRow;
  srcRect.w = destRect.w = bitmap->width;
  srcRect.h = destRect.h = bitmap->height;
  destRect.x = x;
  destRect.y = y;
  SDL_RenderCopyEx(renderer, bitmap->texture, &srcRect, &destRect, 0, 0, SDL_FLIP_NONE);
}

void
drawBitmap(SDL_Renderer *renderer, int x, int y, Bitmap *bitmap, SDL_RendererFlip flip) {
  drawTextureFrame(renderer, bitmap->texture, x, y, bitmap->width, bitmap->height,
                   bitmap->currentRow, bitmap->currentColumn, flip);
}

void
drawBitmapEx(SDL_Renderer *renderer, int x, int y, Bitmap *bitmap, SDL_RendererFlip flip) {
  SDL_Rect srcRect;
  SDL_Rect destRect;
  srcRect.x = bitmap->width * bitmap->currentColumn;
  srcRect.y = bitmap->height * bitmap->currentRow;
  srcRect.w = destRect.w = bitmap->width;
  srcRect.h = destRect.h = bitmap->height;
  destRect.x = x;
  destRect.y = y;
  SDL_SetTextureColorMod(bitmap->texture, bitmap->r, bitmap->g, bitmap->b);
  SDL_SetTextureAlphaMod(bitmap->texture, bitmap->alpha);
  SDL_RenderCopyEx(renderer, bitmap->texture, &srcRect, &destRect, bitmap->angle, 0, flip);
}

void
drawTexture(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int width, int height,
            SDL_RendererFlip flip) {
  SDL_Rect srcRect;
  SDL_Rect destRect;
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
  SDL_Rect srcRect;
  SDL_Rect destRect;
  srcRect.x = width * currentFrame;
  srcRect.y = height * currentRow;
  srcRect.w = destRect.w = width;
  srcRect.h = destRect.h = height;
  destRect.x = x;
  destRect.y = y;
  SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0, 0, flip);
}

