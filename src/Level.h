//
// Created by Jose Gutierrez on 10/18/15.
//

#ifndef HELLO2SDL_LEVEL_H
#define HELLO2SDL_LEVEL_H

#ifdef __APPLE__

#include <SDL2/SDL_render.h>

#else
#include <SDL_render.h>
#endif

#include "GameContext.h"

struct Tileset {
  int firstGridId;
  int tileWidth;
  int tileHeight;
  int spacing;
  int margin;
  int width;
  int height;
  int numColumns;
  char* name;
};

struct Layer {
  void draw(SDL_Renderer *renderer);

  void update(GameContext *gameContext);
};

struct Level {

  void draw(SDL_Renderer *renderer);

  void update(GameContext *gameContext);

  Tileset tilesets[1000];
  Layer layers[1000];
};


#endif //HELLO2SDL_LEVEL_H
