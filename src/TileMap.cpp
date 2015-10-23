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
  for (int i = (y2 < 0 ? -1 : 0); i < (y2 <= 0 ? screenRows : screenRows + 1); ++i) {
    for (int j = (x2 < 0 ? -1 : 0); j < (x2 <= 0 ? screenColumns : screenColumns + 1); ++j) {
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
  velocity.x = 1;
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
  objectLayer->draw(renderer);
}

void TileMap::update(GameContext *gameContext) {
  for (TileLayer *node = tileLayerList; node; node = node->next) {
    node->update(gameContext);
  }
  objectLayer->update(gameContext);
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

  for (auto property = map->properties; property; property = property->next) {
    if (!gameContext->functions.loadTexture(property->name,
                                            property->value,
                                            gameContext->renderer,
                                            &gameContext->permanentMemory)) {
      return false;
    }
  }
  
  auto tileSetList = (TileSet *) 0;
  auto tileSetNode = (TileSet *) 0;

  for (auto tileset = map->ts_head; tileset; tileset = tileset->next) {
    if (!gameContext->functions.loadTexture(tileset->name,
                                            tileset->image->source,
                                            gameContext->renderer,
                                            &gameContext->permanentMemory)) {
      tmx_map_free(map);
      for (tileSetNode = tileSetList; tileSetNode;) {
        auto next = tileSetNode->next;
        freeMemory(&gameContext->longTimeMemory, tileSetNode);
        tileSetNode = next;
      }
      return false;
    }
    auto newTileSetNode = (TileSet *) reserveMemory(&gameContext->longTimeMemory, sizeof(TileSet));
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
    auto tileSetNodeNameLen = strlen(tileset->name);
    tileSetNode->name = (char *) reserveMemory(&gameContext->longTimeMemory, tileSetNodeNameLen + 1);
    strcpy(tileSetNode->name, tileset->name);
    tileSetNode->numColumns = tileSetNode->imageWidth / (tileSetNode->tileWidth + tileSetNode->spacing);
  }

  auto sizeofids = map->height * map->width * sizeof(int32_t);
  auto tileLayerNode = (TileLayer *) 0;
  for (auto tilelayer = map->ly_head; tilelayer; tilelayer = tilelayer->next) {
    if (tilelayer->type == L_LAYER && tilelayer->content.gids) {
      auto newTileLayerNode = (TileLayer *) reserveMemory(&gameContext->longTimeMemory,
                                                          sizeof(TileLayer));
      if (!tileLayerList) {
        tileLayerList = newTileLayerNode;
      }
      if (tileLayerNode) {
        tileLayerNode->next = newTileLayerNode;
      }
      tileLayerNode = newTileLayerNode;
      tileLayerNode->tileGids = (int32_t *) reserveMemory(&gameContext->longTimeMemory, sizeofids);
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
    
    } else if (tilelayer->type == L_OBJGR) {
      auto object = tilelayer->content.objgr->head;
      objectLayer = (ObjectLayer *) reserveMemory(&gameContext->longTimeMemory, sizeof(ObjectLayer));
      auto player = (Player *) reserveMemory(&gameContext->longTimeMemory, sizeof(Player));
      objectLayer->player = player;
      player->entity.position = V2D{(float) object->x, (float) object->y};
      auto numFrames = 0;
      auto textureHeight = 0;
      auto textureWidth = 0;
      auto callbackId = 0;
      auto animSpeed = 0;
      auto textureId = (char *) 0;
      for (auto property = object->properties; property; property = property->next) {
        if (strcmp("numFrames", property->name) == 0) {
          numFrames = atoi(property->value);

        } else if (strcmp("textureHeight", property->name) == 0) {
          textureHeight = atoi(property->value);

        } else if (strcmp("textureWidth", property->name) == 0) {
          textureWidth = atoi(property->value);

        } else if (strcmp("textureId", property->name) == 0) {
          auto textureIdLen = strlen(property->value);
          textureId = (char *) reserveMemory(&gameContext->longTimeMemory, textureIdLen + 1);
          strcpy(textureId, property->value);

        } else if(strcmp("callbackId", property->name) == 0) {
          callbackId = atoi(property->value);

        } else if(strcmp("animSpeed", property->name) == 0) {
          animSpeed = atoi(property->value);
        }
      }
      auto texture = gameContext->functions.getTexture(textureId);
      player->entity.bitmap = Bitmap{texture, textureWidth, textureHeight, numFrames, 1, 1};
      player->entity.velocity = V2D{0,0};
      player->entity.acceleration= V2D{0,0};;
    }
  }

  tmx_map_free(map);

  return true;
}

void
ObjectLayer::update(GameContext *gameContext) {
  player->update(gameContext->userInput);
}

void
ObjectLayer::draw(SDL_Renderer *renderer) {
  player->draw(renderer);
}
