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

struct LoaderParams;
class TextureManager;
class InputHandler;

class Entity {

public:
  enum Type {
    PlayerType, DefaultType, EnemyType
  };

  Entity(Entity::Type type, const LoaderParams& params);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);
  
  void update(InputHandler* inputHandler);
  
  void clean();

  Entity::Type getType() const;

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

  void handleInput(InputHandler* inputHandler);
};


#endif //HELLO2SDL_ENTITY_H
