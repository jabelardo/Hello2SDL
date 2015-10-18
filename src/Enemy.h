//
// Created by Jose Gutierrez on 10/18/15.
//

#ifndef HELLO2SDL_ENEMY_H
#define HELLO2SDL_ENEMY_H


#ifdef __APPLE__

#include <SDL2/SDL_render.h>

#else
#include <SDL_render.h>
#endif

#include "Entity.h"
#include "UserInput.h"

struct Enemy {
  Entity entity;

  void draw(SDL_Renderer *renderer);

  void update(UserInput *userInput);
};


#endif //HELLO2SDL_ENEMY_H
