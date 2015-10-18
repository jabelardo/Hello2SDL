//
// Created by Jose Gutierrez on 10/18/15.
//

#ifdef __APPLE__

#include <SDL2/SDL_timer.h>

#else
#include <SDL_timer.h>
#endif

#include "RenderUtils.h"
#include "Enemy.h"

void
Enemy::draw(SDL_Renderer *renderer) {
  if (entity.velocity.x > 0) {
    drawBitmap(renderer, (int) entity.position.x, (int) entity.position.y, &entity.bitmap,
               SDL_FLIP_HORIZONTAL);
  } else {
    drawBitmap(renderer, (int) entity.position.x, (int) entity.position.y, &entity.bitmap);
  }
}

void
Enemy::update(UserInput *userInput) {
  entity.bitmap.currentFrame = (int) ((SDL_GetTicks() / 100) % entity.bitmap.totalFrames);
  if (entity.position.y < 0) {
    entity.velocity = V2D{2, .33f};
    entity.acceleration = V2D{0, .33f};

  } else if (entity.position.y > 400) {
    entity.velocity = V2D{2, -.33f};
    entity.acceleration = V2D{0, -.33f};
  }
  if (entity.position.x > 640) {
    entity.position.x = 0;
  }
  entity.velocity += entity.acceleration;
  entity.position += entity.velocity;
}