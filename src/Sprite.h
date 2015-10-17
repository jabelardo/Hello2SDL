//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_SPRITE_H
#define HELLO2SDL_SPRITE_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "Vector2D.h"
#include "TextureId.h"

struct LoaderParams;
class TextureManager;
class InputHandler;

struct Sprite {

  Sprite(const LoaderParams& params);

  TextureId textureId;
  Vector2D position;
  int width;
  int height;
  int currentFrame;
  int currentRow;
  int totalFrames;

  void draw(TextureManager* textureManager, SDL_Renderer* renderer,
            SDL_RendererFlip flip = SDL_FLIP_NONE);

  static bool checkCollision(const Sprite& sprite1, const Sprite& sprite2);

};


#endif //HELLO2SDL_SPRITE_H
