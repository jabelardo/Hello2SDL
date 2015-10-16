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
