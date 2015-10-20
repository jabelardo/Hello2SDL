//
// Created by Jose Gutierrez on 10/18/15.
//

#ifndef HELLO2SDL_TILEMMAP_H
#define HELLO2SDL_TILEMMAP_H

#ifdef __APPLE__

#include <SDL2/SDL_render.h>

#else
#include <SDL_render.h>
#endif

#include "GameContext.h"
#include "V2D.h"
#include "tmx.h"

struct TileSet {
  char* name;
  int firstGid;
  int tileCount;
  int tileWidth;
  int tileHeight;
  int imageWidth;
  int imageHeight;
  int spacing;
  int margin;
  int numColumns;
  TileSet* next;
  SDL_Texture *texture;
};

struct TileLayer {
  int tileWidth;
  int tileHeight;
  V2D position;
  V2D velocity;

  int numColumns;
  int numRows;

  TileSet* tileSetList;
  size_t tileGidsCount;
  int32_t* tileGids;
  TileLayer* next;

  void draw(SDL_Renderer *renderer);

  void update(GameContext *gameContext);

  TileSet* getTileSetById(int tileId);
};

struct TileMap {
  int width;
  int height;
  int tileWidth;
  int tileHeight;
  TileLayer* tileLayerList;

  bool init(GameContext *gameContext, const char *mapfile);

  void draw(SDL_Renderer *renderer);

  void update(GameContext *gameContext);
};

#endif //HELLO2SDL_TILEMMAP_H