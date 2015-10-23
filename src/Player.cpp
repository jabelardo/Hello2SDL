//
// Created by Jose Gutierrez on 10/17/15.
//

#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include "Player.h"
#include "RenderUtils.h"

void
Player::draw(SDL_Renderer *renderer) {
  if (entity.velocity.x > 0) {
    drawBitmap(renderer, (int) entity.position.x, (int) entity.position.y, &entity.bitmap,
               SDL_FLIP_HORIZONTAL);
  } else {
    drawBitmap(renderer, (int) entity.position.x, (int) entity.position.y, &entity.bitmap);
  }
}

void
Player::update(UserInput *userInput) {
  entity.bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity.bitmap.totalFrames);
  entity.velocity = V2D{0, 0};

  V2D target = {(float) userInput->mousePositionX, (float) userInput->mousePositionY};
  entity.velocity = (target - entity.position) / 50;

  entity.velocity += entity.acceleration;
  entity.position += entity.velocity;
}
