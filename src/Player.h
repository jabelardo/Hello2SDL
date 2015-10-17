//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_PLAYER_H
#define HELLO2SDL_PLAYER_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "UserInput.h"
#include "Entity.h"

struct LoaderParams;
class TextureManager;
class InputHandler;

struct Player : Sprite {
  Vector2D velocity;
  Vector2D acceleration;

  Player(const LoaderParams& params);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);

  void update(UserInput *userInput);
};


#endif //HELLO2SDL_PLAYER_H
