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
#include "UserInput.h"

void
Entity::draw(SDL_Renderer* renderer) {
  if (sprite.velocity.x > 0) {
    sprite.draw(renderer, SDL_FLIP_HORIZONTAL);
  } else {
    sprite.draw(renderer);
  }
}

void
Entity::update(UserInput *userInput) {
  sprite.currentFrame = (int) ((SDL_GetTicks() / 100) % sprite.totalFrames);
  if (sprite.position.y < 0) {
    sprite.velocity = Vector2D{2, .33f};
    sprite.acceleration = Vector2D{0, .33f};

  } else if (sprite.position.y > 400) {
    sprite.velocity = Vector2D{2, -.33f};
    sprite.acceleration = Vector2D{0, -.33f};
  }
  if (sprite.position.x > 640) {
    sprite.position.x = 0;
  }
  sprite.velocity += sprite.acceleration;
  sprite.position += sprite.velocity;
}
