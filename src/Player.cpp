//
// Created by Jose Gutierrez on 10/17/15.
//


#ifdef __APPLE__

#include <SDL2/SDL_timer.h>
#include <assert.h>
#include <SDL2/SDL_scancode.h>

#else
#include <SDL_timer.h>
#endif

#include "Player.h"
#include "UserInput.h"

void
Player::draw(SDL_Renderer* renderer) {
  if (sprite.velocity.x > 0) {
    sprite.draw(renderer, SDL_FLIP_HORIZONTAL);
  } else {
    sprite.draw(renderer);
  }
}

void
Player::update(UserInput *userInput) {
  sprite.currentFrame = (int) ((SDL_GetTicks() / 100) % sprite.totalFrames);
  sprite.velocity = Vector2D{0, 0};

  auto target = Vector2D{(float) userInput->mousePositionX, (float) userInput->mousePositionY};
  sprite.velocity = (target - sprite.position) / 50;

  sprite.velocity += sprite.acceleration;
  sprite.position += sprite.velocity;
}
