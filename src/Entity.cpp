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
#include "UserInput.h"

Entity::Entity(Entity::Type type, const LoaderParams &params)
    : type(type), sprite(params), velocity(), acceleration() {
  switch (type) {
    case Entity::ENEMY_TYPE: {
      velocity = Vector2D{2, .33f};
      acceleration = Vector2D{0, .33f};
      break;
    }
    case PLAYER_TYPE: {
      velocity = Vector2D{0, 0};
      acceleration = Vector2D{0, 0};
      break;
    }
  }
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
Entity::update(UserInput *userInput) {
  sprite.setCurrentFrame((int) ((SDL_GetTicks() / 100) % sprite.getTotalFrames()));
  switch (type) {
    case Entity::PLAYER_TYPE: {
      velocity = Vector2D{0, 0};
      handleInput(userInput);
      break;
    }
    case Entity::ENEMY_TYPE: {
      if (sprite.getPosition().y < 0) {
        velocity = Vector2D{2, .33f};
        acceleration = Vector2D{0, .33f};

      } else if (sprite.getPosition().y > 400) {
        velocity = Vector2D{2, -.33f};
        acceleration = Vector2D{0, -.33f};
      }
      if (sprite.getPosition().x > 640) {
        auto pos = sprite.getPosition();
        pos.x = 0;
        sprite.setPosition(pos);
      }
      break;
    }
  }
  velocity += acceleration;
  sprite.setPosition(sprite.getPosition() + velocity);
}

void
Entity::clean() {

}

Entity::Type
Entity::getType() const {
  return type;
}

void Entity::handleInput(UserInput *userInput) {
  assert(type == PLAYER_TYPE);

  auto target = Vector2D{(float) userInput->mousePositionX, (float) userInput->mousePositionY};
  velocity = (target - sprite.getPosition()) / 50;
}

const Sprite &Entity::getSprite() const {
  return sprite;
}
