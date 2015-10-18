//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_RENDERUTILS_H
#define HELLO2SDL_RENDERUTILS_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

void draw(SDL_Renderer * renderer, SDL_Texture * texture, int x, int y, int width, int height,
          SDL_RendererFlip flip = SDL_FLIP_NONE);

void drawFrame(SDL_Renderer * renderer, SDL_Texture * texture, int x, int y, int width, int height,
               int currentRow, int currentFrame, SDL_RendererFlip flip = SDL_FLIP_NONE);

#endif //HELLO2SDL_RENDERUTILS_H
