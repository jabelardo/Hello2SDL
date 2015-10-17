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
#include "LoaderParams.h"
#include "TextureManager.h"
#include "UserInput.h"

Player::Player(const LoaderParams &params)
    : Sprite(params), velocity(0, 0), acceleration(0, 0) {
}

void
Player::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (velocity.x > 0) {
    Sprite::draw(textureManager, renderer, SDL_FLIP_HORIZONTAL);
  } else {
    Sprite::draw(textureManager, renderer);
  }
}

void
Player::update(UserInput *userInput) {
  currentFrame = (int) ((SDL_GetTicks() / 100) % totalFrames);
  velocity = Vector2D{0, 0};

  auto target = Vector2D{(float) userInput->mousePositionX, (float) userInput->mousePositionY};
  velocity = (target - position) / 50;

  velocity += acceleration;
  position += velocity;
}
