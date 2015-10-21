//
// Created by Jose Gutierrez on 10/18/15.
//

#include "TileMap.h"
#include "tmx.h"
#include "RenderUtils.h"

#include "tmx.h"

void
TileLayer::draw(SDL_Renderer *renderer) {
  auto x = (int) position.x / tileWidth;
  auto y = (int) position.y / tileHeight;
  auto x2 = (int) position.x % tileWidth;
  auto y2 = (int) position.y % tileHeight;
  for (int i = 0 - (y2 < 0 ? 1 : 0); i < screenRows + (y2 > 0 ? 1 : 0); ++i) {
    for (int j = 0 - (x2 < 0 ? 1 : 0); j < screenColumns + (x2 > 0 ? 1 : 0); ++j) {
      int tileX = j + x;
      if (tileX >= mapWidth) {
        tileX -= mapWidth;
      } else if (tileX < 0) {
        tileX += mapWidth;
      }
      int tileY = i + y;
      if (tileY >= mapHeight) {
        tileY -= mapHeight;
      } else if (tileY < 0) {
        tileY += mapHeight;
      }
      int tileIdx = tileY * mapWidth + tileX;
      if (tileIdx >= tileGidsCount) {
        continue;
      }
      int32_t tileId = tileGids[tileIdx];
      if (tileId == 0) {
        continue;
      }
      TileSet *tileSet = getTileSetById(tileId);
      if (tileSet == 0) {
        continue;
      }
      auto texture = tileSet->texture;
      auto currentFrame = (tileId - 1 - (tileSet->firstGid - 1)) % tileSet->numColumns;
      auto currentRow = (tileId - 1 - (tileSet->firstGid - 1)) / tileSet->numColumns;
      auto bitmap = Bitmap{texture, tileWidth, tileHeight, 0, currentFrame, currentRow};
      auto x3 = (j * tileWidth) - x2;
      auto y3 = (i * tileHeight) - y2;
      drawTile(renderer, 2, 2, x3, y3, &bitmap);
    }
  }
}

void
TileLayer::update(GameContext *gameContext) {
  velocity.x = -2;
//  position.x = 16;
  position += velocity;
  if (position.x == mapWidth * tileWidth || position.x == -mapWidth * tileWidth) {
    position.x = 0;
  }
  if (position.y == mapHeight * tileHeight || position.y == -mapHeight * tileHeight) {
    position.y = 0;
  }
}

TileSet *
TileLayer::getTileSetById(int tileId) {
  for (TileSet *node = tileSetList; node; node = node->next) {
    if (tileId >= node->firstGid && tileId < node->firstGid + node->tileCount) {
      return node;
    }
  }
  return 0;
}

void
TileMap::draw(SDL_Renderer *renderer) {
  for (TileLayer *node = tileLayerList; node; node = node->next) {
    node->draw(renderer);
  }
}

void TileMap::update(GameContext *gameContext) {
  for (TileLayer *node = tileLayerList; node; node = node->next) {
    node->update(gameContext);
  }
}

bool
TileMap::init(GameContext *gameContext, const char *mapfile) {

  auto map = tmx_load(mapfile);
  if (!map) {
    return false;
  }

  width = map->width;
  height = map->height;
  tileWidth = map->tile_width;
  tileHeight = map->tile_height;
  tileLayerList = 0;

  TileSet *tileSetList = 0;
  TileSet *tileSetNode = 0;

  for (tmx_tileset *tileset = map->ts_head; tileset; tileset = tileset->next) {
    if (!gameContext->functions.loadTexture(
        tileset->name, tileset->image->source, gameContext->renderer)) {
      tmx_map_free(map);
      // TODO: free the tileSetList !!!!
      return false;
    }
    TileSet *newTileSetNode = (TileSet *) reserveMemory(&gameContext->permanentMemory,
                                                        sizeof(TileSet));
    if (!tileSetList) {
      tileSetList = newTileSetNode;
    }
    if (tileSetNode) {
      tileSetNode->next = newTileSetNode;
    }
    tileSetNode = newTileSetNode;
    tileSetNode->imageWidth = tileset->image->width;
    tileSetNode->imageHeight = tileset->image->height;
    tileSetNode->firstGid = tileset->firstgid;
    tileSetNode->tileCount = tileset->tile_count;
    tileSetNode->tileWidth = tileset->tile_width;
    tileSetNode->tileHeight = tileset->tile_height;
    tileSetNode->spacing = tileset->spacing;
    tileSetNode->margin = tileset->margin;
    tileSetNode->texture = gameContext->functions.getTexture(tileset->name);
    size_t tileSetNodeNameLen = strlen(tileset->name);
    tileSetNode->name = (char *) reserveMemory(&gameContext->permanentMemory, tileSetNodeNameLen + 1);
    strcpy(tileSetNode->name, tileset->name);
    tileSetNode->numColumns = tileSetNode->imageWidth / (tileSetNode->tileWidth + tileSetNode->spacing);
  }

  size_t sizeofids = map->height * map->width * sizeof(int32_t);
  TileLayer *tileLayerNode = 0;
  for (tmx_layer *tilelayer = map->ly_head; tilelayer; tilelayer = tilelayer->next) {
    if (tilelayer->type == L_LAYER && tilelayer->content.gids) {
      TileLayer *newTileLayerNode = (TileLayer *) reserveMemory(&gameContext->permanentMemory,
                                                                sizeof(TileLayer));
      if (!tileLayerList) {
        tileLayerList = newTileLayerNode;
      }
      if (tileLayerNode) {
        tileLayerNode->next = newTileLayerNode;
      }
      tileLayerNode = newTileLayerNode;
      tileLayerNode->tileGids = (int32_t *) reserveMemory(&gameContext->permanentMemory, sizeofids);
      memcpy(tileLayerNode->tileGids, tilelayer->content.gids, sizeofids);
      tileLayerNode->tileGidsCount = map->height * map->width;
      tileLayerNode->tileWidth = tileWidth;
      tileLayerNode->tileHeight = tileHeight;
      tileLayerNode->screenWidth = gameContext->screenWidth;
      tileLayerNode->screenHeight = gameContext->screenWidth;
      tileLayerNode->screenColumns = gameContext->screenWidth / tileWidth;
      tileLayerNode->screenRows = gameContext->screenHeight / tileHeight;
      tileLayerNode->mapWidth = map->width;
      tileLayerNode->mapHeight = map->height;
      tileLayerNode->tileSetList = tileSetList;
    }
  }

  tmx_map_free(map);

  return true;
}

