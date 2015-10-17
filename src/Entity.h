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

struct Entity {

  Sprite sprite;

  void draw(SDL_Renderer* renderer);
  
  void update(UserInput *userInput);
};


#endif //HELLO2SDL_ENTITY_H
