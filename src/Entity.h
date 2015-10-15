//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_ENTITY_H
#define HELLO2SDL_ENTITY_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif
#include "Vector2D.h"
#include "LoaderParams.h"
#include "TextureManager.h"

class Entity {

public:
  enum Type {
    PlayerType, DefaultType, EnemyType
  };

  Entity(Entity::Type type, const LoaderParams& params);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);
  
  void frameUpdate();
  
  void clean();

  Entity::Type getType();

  Vector2D getPosition();

  void setPosition(const Vector2D& position);

  Vector2D getVelocity();

  void setVelocity(const Vector2D& velocity);

  Vector2D getAcceleration();

  void setAcceleration(const Vector2D& acceleration);

private:
  Entity::Type type;
  int textureId;
  int currentFrame;
  int currentRow;
  int width;
  int height;
  Vector2D position;
  Vector2D velocity;
  Vector2D acceleration;
};


#endif //HELLO2SDL_ENTITY_H
