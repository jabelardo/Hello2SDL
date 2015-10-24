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

struct Entity;

struct ObjectLayer {
  Entity* player;
  V2D playerInitialPosition;
};

struct TileLayer {
  int screenWidth;
  int screenHeight;
  int mapWidth;
  int mapHeight;
  int tileWidth;
  int tileHeight;
  V2D position;
  V2D velocity;

  int screenColumns;
  int screenRows;

  TileSet* tileSetList;
  size_t tileGidsCount;
  int32_t* tileGids;
  TileLayer* next;
};

struct TileMap {
  int width;
  int height;
  int tileWidth;
  int tileHeight;
  TileLayer* tileLayerList;
  ObjectLayer* objectLayer;
};

#endif //HELLO2SDL_TILEMMAP_H
