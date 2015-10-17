//
// Created by Jose Gutierrez on 10/14/15.
//
#ifdef __APPLE__

#include <SDL2/SDL_timer.h>
#include <assert.h>
#include <SDL2/SDL_scancode.h>

#else
#include <SDL_timer.h>
#endif

#include "Entity.h"
#include "LoaderParams.h"
#include "TextureManager.h"
#include "UserInput.h"

Entity::Entity(const LoaderParams &params) 
  : Sprite(params), velocity(0, 0), acceleration(0, 0) {
}

void
Entity::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (velocity.x > 0) {
    Sprite::draw(textureManager, renderer, SDL_FLIP_HORIZONTAL);
  } else {
    Sprite::draw(textureManager, renderer);
  }
}

void
Entity::update(UserInput *userInput) {
  currentFrame = (int) ((SDL_GetTicks() / 100) % totalFrames);
  if (position.y < 0) {
    velocity = Vector2D{2, .33f};
    acceleration = Vector2D{0, .33f};

  } else if (position.y > 400) {
    velocity = Vector2D{2, -.33f};
    acceleration = Vector2D{0, -.33f};
  }
  if (position.x > 640) {
    position.x = 0;
  }
  velocity += acceleration;
  position += velocity;
}
