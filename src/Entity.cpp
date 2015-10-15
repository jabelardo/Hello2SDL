//
// Created by Jose Gutierrez on 10/14/15.
//
#ifdef __APPLE__
#include <SDL2/SDL_timer.h>
#else
#include <SDL_timer.h>
#endif

#include "Entity.h"

Entity::Entity(Entity::Type type, const LoaderParams& params)
: type(type), textureId(params.textureId), currentFrame(1), currentRow(1), width(params.width),
  height(params.height), position{params.x, params.y}, velocity{0, 0}, acceleration{0, 0} {
}


Entity::Type
Entity::getType() {
  return type;
}

Vector2D
Entity::getPosition() {
  return position;
}

void
Entity::setPosition(const Vector2D& position) {
  this->position = position;
}

Vector2D
Entity::getVelocity() {
  return velocity;
}

void
Entity::setVelocity(const Vector2D& velocity) {
  this->velocity = velocity;
}

Vector2D
Entity::getAcceleration() {
  return acceleration;
}

void
Entity::setAcceleration(const Vector2D& acceleration) {
  this->acceleration = acceleration;
}

void
Entity::draw(TextureManager* textureManager, SDL_Renderer* renderer) {
  textureManager->drawFrame(textureId, (int) position.x, (int) position.y, width, height,
                            currentRow, currentFrame, renderer);
}

void
Entity::frameUpdate() {
  currentFrame = (int)((SDL_GetTicks() / 100) % 6);
}

void
Entity::clean() {

}