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
#include "LoaderParams.h"
#include "TextureManager.h"
#include "InputHandler.h"

Entity::Entity(Entity::Type type, const LoaderParams &params)
    : type(type), sprite(params), velocity{0, 0}, acceleration{0, 0} {
}

void
Entity::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  if (velocity.x > 0) {
    sprite.draw(textureManager, renderer, SDL_FLIP_HORIZONTAL);
  } else {
    sprite.draw(textureManager, renderer);
  }
}

void
Entity::update(InputHandler *inputHandler) {
  switch (type) {
    case Entity::PLAYER_TYPE: {
      velocity = Vector2D{0, 0};
      handleInput(inputHandler);
      velocity += acceleration;
      sprite.setPosition(sprite.getPosition() + velocity);
    }
      break;
    case Entity::DEFAULT_TYPE: {
      sprite.setPosition(sprite.getPosition() - Vector2D{1, 0});
    }
      break;
    case Entity::ENEMY_TYPE: {
      sprite.setPosition(sprite.getPosition() + Vector2D{1, 1});
    }
      break;
  }
  sprite.setCurrentFrame((int) ((SDL_GetTicks() / 100) % sprite.getTotalFrames()));
}

void
Entity::clean() {

}

Entity::Type
Entity::getType() const {
  return type;
}

void Entity::handleInput(InputHandler *inputHandler) {
  assert(type == PLAYER_TYPE);

  auto target = inputHandler->getMousePosition();
  velocity = (target - sprite.getPosition()) / 50;

//  if (inputHandler->isKeyDown(SDL_SCANCODE_RIGHT)) {
//    velocity += Vector2D{2, 0};
//  }
//  if (inputHandler->isKeyDown(SDL_SCANCODE_LEFT)) {
//    velocity -= Vector2D{2, 0};
//  }
//  if (inputHandler->isKeyDown(SDL_SCANCODE_UP)) {
//    velocity -= Vector2D{0, 2};
//  }
//  if (inputHandler->isKeyDown(SDL_SCANCODE_DOWN)) {
//    velocity += Vector2D{0, 2};
//  }

//  auto vect = Vector2D{0, 0};
//  if (inputHandler->getMouseButtonState(InputHandler::LeftMouseButton)) {
//    if (velocity.x > 0) {
//      vect += Vector2D{.025, 0};
//    }
//    if (velocity.x < 0) {
//      vect -= Vector2D{.025, 0};
//    }
//    if (velocity.y > 0) {
//      vect += Vector2D{0, .025};
//    }
//    if (velocity.y < 0) {
//      vect -= Vector2D{0, .025};
//    }
//  }
//  if (inputHandler->getMouseButtonState(InputHandler::RightMouseButton)) {
//    if (velocity.x > 0) {
//      vect -= Vector2D{.025, 0};
//    }
//    if (velocity.x < 0) {
//      vect += Vector2D{.025, 0};
//    }
//    if (velocity.y > 0) {
//      vect -= Vector2D{0, .025};
//    }
//    if (velocity.y < 0) {
//      vect += Vector2D{0, .025};
//    }
//  }
//  acceleration += vect;
}
