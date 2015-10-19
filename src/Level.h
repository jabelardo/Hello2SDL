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
#include "V2D.h"


//struct mx_map {
//  /* <map> (Head of the data structure) */
//  enum tmx_map_orient orient;
//  unsigned int width, height;
//  unsigned int tile_width, tile_height;
//  int backgroundcolor;
//  /* bytes : RGB */
//  enum tmx_map_renderorder renderorder;
//};

struct Tileset {
////  map_renderorder renderorder;
//  int backgroundcolor;
  int firstGridId;
//  int tileWidth;
//  int tileHeight;
//  int spacing;
//  int margin;
//  int width;
//  int height;
  int numColumns;
//  char* name;
};

struct TileLayer {
  int numColumns; // 640 / tileSize
  int numRows; // 480 / tileSize
  int tileSize;
  V2D position;
  V2D velocity;

  size_t tilesetsSize;
  Tileset* tilesets;
  int tileIds[32][32];

//  const std::vector<Tileset> &m_tilesets;
//  std::vector<std::vector<int>> m_tileIDs;

  bool init(GameContext *gameContext);

  void draw(GameContext *gameContext, SDL_Renderer *renderer);

  void update(GameContext *gameContext);

  Tileset* getTilesetById(int tileId);
};

//struct Level {
//
//  void draw(SDL_Renderer *renderer);
//
//  void update(GameContext *gameContext);
//
//  Tileset tilesets[1000];
//  Layer layers[1000];
//};


#endif //HELLO2SDL_LEVEL_H
