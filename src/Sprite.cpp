//
// Created by Jose Gutierrez on 10/15/15.
//

#include "Sprite.h"
#include "RenderUtils.h"

void
Sprite::draw(SDL_Renderer* renderer, SDL_RendererFlip flip) {
  drawFrame(renderer, texture, (int) position.x, (int) position.y, width, height, currentRow,
            currentFrame, flip);
}

bool
Sprite::checkCollision(const Sprite &sprite1, const Sprite &sprite2) {
  auto leftA = sprite1.position.x;
  auto rightA = sprite1.position.x + sprite1.width;
  auto topA = sprite1.position.y;
  auto bottomA = sprite1.position.y + sprite1.height;
  //Calculate the sides of rect B
  auto leftB = sprite2.position.x;
  auto rightB = sprite2.position.x + sprite2.width;
  auto topB = sprite2.position.y;
  auto bottomB = sprite2.position.y + sprite2.height;
  //If any of the sides from A are outside of B
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}
