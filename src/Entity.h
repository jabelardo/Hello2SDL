//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_ENTITY_H
#define HELLO2SDL_ENTITY_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "Sprite.h"
#include "UserInput.h"

struct LoaderParams;
class TextureManager;
class InputHandler;

struct Entity : Sprite {
  Vector2D velocity;
  Vector2D acceleration;

  Entity(const LoaderParams& params);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);
  
  void update(UserInput *userInput);
};


#endif //HELLO2SDL_ENTITY_H
