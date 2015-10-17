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

struct Sprite {

  SDL_Texture* texture;
  Vector2D position;
  int width;
  int height;
  int totalFrames;
  int currentFrame;
  int currentRow;
  Vector2D velocity;
  Vector2D acceleration;

  void draw(SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

  static bool checkCollision(const Sprite& sprite1, const Sprite& sprite2);

};


#endif //HELLO2SDL_SPRITE_H
