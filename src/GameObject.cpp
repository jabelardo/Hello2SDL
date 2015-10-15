//
// Created by Jose Gutierrez on 10/14/15.
//

#include "GameObject.h"

void
GameObject::load(int textureId, int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
  this->textureId = textureId;
  this->currentRow = 1;
  this->currentFrame = 1;
}

void
GameObject::draw(TextureManager* textureManager, SDL_Renderer* renderer) {
  textureManager->drawFrame(textureId, x, y, width, height, currentRow, currentFrame, renderer);
}

void
GameObject::update() {
  currentFrame = int(((SDL_GetTicks() / 100) % 6));
  x += 1;
}

void
GameObject::clean() {

}