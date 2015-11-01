//
// Created by Jose Gutierrez on 10/18/15.
//

#ifndef HELLO2SDL_TILEMMAP_H
#define HELLO2SDL_TILEMMAP_H

#include <SDL2/SDL_render.h>
#include "V2D.h"
#include "Entity.h"

struct GameContext;
struct GameMemory;
struct GameMemory;
struct PlatformConfig;
struct PlayState;
struct ScrollingBackground;
struct SDL_Renderer;
struct SDL_Texture;
struct UserInput;

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

struct ObjectLayer {
  EntityNode* entityList;
  ObjectLayer* next;
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
  bool collidable;

  TileSet* tileSetList;
  size_t tileGidsCount;
  int32_t* tileGids;
  SDL_Texture* texture;
  TileLayer* next;
};

struct TileMap {
  int width;
  int height;
  int tileWidth;
  int tileHeight;
  Entity* player;
  V2D playerInitialPosition;
  TileLayer* tileLayerList;
  ObjectLayer* objectLayerList;
  ScrollingBackground *scrollingBackground;
};

bool initTileMap(TileMap *tileMap, const char *mapfileName, GameContext *gameContext,
                 SDL_Renderer *renderer, GameMemory* gameMemory, PlatformConfig *platformConfig);

void updateTileMap(TileMap *tileMap, PlayState *playState, GameContext *gameContext,
                   UserInput *userInput, GameMemory *gameMemory);

void drawTileMap(TileMap *tileMap, SDL_Renderer *renderer);

#endif //HELLO2SDL_TILEMMAP_H
