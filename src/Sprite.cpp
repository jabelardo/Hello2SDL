//
// Created by Jose Gutierrez on 10/15/15.
//

#include "Sprite.h"
#include "LoaderParams.h"
#include "TextureManager.h"

Sprite::Sprite(const LoaderParams &params)
: textureId(params.textureId),
  position{params.x, params.y},
  width(params.width), height(params.height),
  currentFrame(1), currentRow(1), totalFrames(params.frames) {
}

void
Sprite::draw(TextureManager *textureManager, SDL_Renderer *renderer, SDL_RendererFlip flip) {
  textureManager->drawFrame(textureId, (int) position.x, (int) position.y, width, height,
                            currentRow, currentFrame, renderer, flip);
}

const Vector2D&
Sprite::getPosition() const {
  return position;
}

void
Sprite::setPosition(const Vector2D& position) {
  this->position = position;
}

int
Sprite::getCurrentFrame() const {
  return currentFrame;
}

void
Sprite::setCurrentFrame(int currentFrame) {
  this->currentFrame = currentFrame;
}

int
Sprite::getWidth() const {
  return width;
}

int
Sprite::getHeight() const {
  return height;
}

TextureId
Sprite::getTextureId() const {
  return textureId;
}

int
Sprite::getCurrentRow() const {
  return currentRow;
}

int
Sprite::getTotalFrames() const {
  return totalFrames;
}

bool
Sprite::checkCollision(const Sprite &sprite1, const Sprite &sprite2) {
  auto leftA = sprite1.getPosition().x;
  auto rightA = sprite1.getPosition().x + sprite1.getWidth();
  auto topA = sprite1.getPosition().y;
  auto bottomA = sprite1.getPosition().y + sprite1.getHeight();
  //Calculate the sides of rect B
  auto leftB = sprite2.getPosition().x;
  auto rightB = sprite2.getPosition().x + sprite2.getWidth();
  auto topB = sprite2.getPosition().y;
  auto bottomB = sprite2.getPosition().y + sprite2.getHeight();
  //If any of the sides from A are outside of B
  if (bottomA <= topB) { return false; }
  if (topA >= bottomB) { return false; }
  if (rightA <= leftB) { return false; }
  if (leftA >= rightB) { return false; }
  return true;
}
