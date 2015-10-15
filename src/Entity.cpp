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
    : type(type), textureId(params.textureId), currentFrame(1), currentRow(1), width(params.width),
      height(params.height), position{params.x, params.y}, velocity{0, 0}, acceleration{0, 0} {
}

void
Entity::draw(TextureManager *textureManager, SDL_Renderer *renderer) {
  textureManager->drawFrame(textureId, (int) position.x, (int) position.y, width, height,
                            currentRow, currentFrame, renderer);
}

void
Entity::update(InputHandler *inputHandler) {
  switch (type) {
    case Entity::PlayerType: {
      velocity = Vector2D{0, 0};
      handleInput(inputHandler);
      velocity += acceleration;
      position += velocity;
    }
      break;
    case Entity::DefaultType: {
      position -= Vector2D{1, 0};
    }
      break;
    case Entity::EnemyType: {
      position += Vector2D{1, 1};
    }
      break;
  }
  currentFrame = (int) ((SDL_GetTicks() / 100) % 6);
}

void
Entity::clean() {

}

Entity::Type
Entity::getType() const {
  return type;
}

void Entity::handleInput(InputHandler *inputHandler) {
  assert(type == PlayerType);

  auto vec = inputHandler->getMousePosition();
  velocity = (vec - position) / 100;

  if (inputHandler->isKeyDown(SDL_SCANCODE_RIGHT)) {
    velocity += Vector2D{2, 0};
  }
  if (inputHandler->isKeyDown(SDL_SCANCODE_LEFT)) {
    velocity -= Vector2D{2, 0};
  }
  if (inputHandler->isKeyDown(SDL_SCANCODE_UP)) {
    velocity -= Vector2D{0, 2};
  }
  if (inputHandler->isKeyDown(SDL_SCANCODE_DOWN)) {
    velocity += Vector2D{0, 2};
  }

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
