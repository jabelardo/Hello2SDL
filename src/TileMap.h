//
// Created by Jose Gutierrez on 10/18/15.
//

#ifndef HELLO2SDL_TILEMMAP_H
#define HELLO2SDL_TILEMMAP_H

#include <SDL2/SDL_render.h>
#include "V2D.h"
#include "Entity.h"

struct ScrollingBackground;

struct TileMap {
  int width;
  int height;
  int tileWidth;
  int tileHeight;
  EntityNode* entityList;
  ScrollingBackground *scrollingBackground;
};

bool loadTileMap(TileMap *tileMap, const char *mapfileName, GameContext *gameContext, SDL_Renderer *renderer,
                 GameMemory *gameMemory, EntityNode** freeEntityList);


#endif //HELLO2SDL_TILEMMAP_H
