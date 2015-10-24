//
// Created by Jose Gutierrez on 10/18/15.
//

#include "TileMap.h"
#include "tmx.h"
#include "RenderUtils.h"
#include "Entity.h"

#include "tmx.h"

TileSet *
getTileSetById(TileLayer* tileLayer, int tileId) {
  for (TileSet *node = tileLayer->tileSetList; node; node = node->next) {
    if (tileId >= node->firstGid && tileId < node->firstGid + node->tileCount) {
      return node;
    }
  }
  return 0;
}

void
drawTileLayer(TileLayer* tileLayer, SDL_Renderer *renderer) {
  int x = (int) tileLayer->position.x / tileLayer->tileWidth;
  int y = (int) tileLayer->position.y / tileLayer->tileHeight;
  int x2 = (int) tileLayer->position.x % tileLayer->tileWidth;
  int y2 = (int) tileLayer->position.y % tileLayer->tileHeight;
  for (int i = (y2 < 0 ? -1 : 0); i < (y2 <= 0 ? tileLayer->screenRows : tileLayer->screenRows + 1); ++i) {
    for (int j = (x2 < 0 ? -1 : 0); j < (x2 <= 0 ? tileLayer->screenColumns : tileLayer->screenColumns + 1); ++j) {
      int tileX = j + x;
      if (tileX >= tileLayer->mapWidth) {
        tileX -= tileLayer->mapWidth;
      } else if (tileX < 0) {
        tileX += tileLayer->mapWidth;
      }
      int tileY = i + y;
      if (tileY >= tileLayer->mapHeight) {
        tileY -= tileLayer->mapHeight;
      } else if (tileY < 0) {
        tileY += tileLayer->mapHeight;
      }
      int tileIdx = tileY * tileLayer->mapWidth + tileX;
      if (tileIdx >= tileLayer->tileGidsCount) {
        continue;
      }
      int32_t tileId = tileLayer->tileGids[tileIdx];
      if (tileId == 0) {
        continue;
      }
      TileSet *tileSet = getTileSetById(tileLayer, tileId);
      if (tileSet == 0) {
        continue;
      }
      SDL_Texture *texture = tileSet->texture;
      int currentFrame = (tileId - 1 - (tileSet->firstGid - 1)) % tileSet->numColumns;
      int currentRow = (tileId - 1 - (tileSet->firstGid - 1)) / tileSet->numColumns;
      Bitmap bitmap = {texture, tileLayer->tileWidth, tileLayer->tileHeight, 0, currentFrame, currentRow};
      int x3 = (j * tileLayer->tileWidth) - x2;
      int y3 = (i * tileLayer->tileHeight) - y2;
      drawTile(renderer, 2, 2, x3, y3, &bitmap);
    }
  }
}

void
drawObjectLayer(ObjectLayer* objectLayer, SDL_Renderer *renderer) {
  drawEntity(objectLayer->player, renderer);
}

void
drawTileMap(TileMap* tileMap, SDL_Renderer *renderer) {
  for (TileLayer *node = tileMap->tileLayerList; node; node = node->next) {
    drawTileLayer(node, renderer);
  }
  drawObjectLayer(tileMap->objectLayer, renderer);
}

void
updateTileLayer(TileLayer *tileLayer, GameContext *gameContext) {
  tileLayer->velocity.x = 1;
  tileLayer->position += tileLayer->velocity;
  if (tileLayer->position.x == tileLayer->mapWidth * tileLayer->tileWidth ||
      tileLayer->position.x == -tileLayer->mapWidth * tileLayer->tileWidth) {
    tileLayer->position.x = 0;
  }
  if (tileLayer->position.y == tileLayer->mapHeight * tileLayer->tileHeight ||
      tileLayer->position.y == -tileLayer->mapHeight * tileLayer->tileHeight) {
    tileLayer->position.y = 0;
  }
}
void
updateObjectLayer(ObjectLayer* objectLayer, GameContext *gameContext) {
  updateEntity(objectLayer->player, gameContext->userInput);
}

void
updateTileMap(TileMap* tileMap, GameContext *gameContext) {
  for (TileLayer *node = tileMap->tileLayerList; node; node = node->next) {
    updateTileLayer(node, gameContext);
  }
  updateObjectLayer(tileMap->objectLayer, gameContext);
}

bool
initTileMap(TileMap *tileMap, GameContext *gameContext, const char *mapfile) {

  tmx_map *map = tmx_load(mapfile);
  if (!map) {
    return false;
  }

  tileMap->width = map->width;
  tileMap->height = map->height;
  tileMap->tileWidth = map->tile_width;
  tileMap->tileHeight = map->tile_height;
  tileMap->tileLayerList = 0;

  for (tmx_property *property = map->properties; property; property = property->next) {
    if (!gameContext->functions.loadTexture(property->name,
                                            property->value,
                                            gameContext->renderer,
                                            &gameContext->permanentMemory)) {
      return false;
    }
  }

  TileSet *tileSetList = 0;
  TileSet *tileSetNode = 0;

  for (tmx_tileset *tileset = map->ts_head; tileset; tileset = tileset->next) {
    if (!gameContext->functions.loadTexture(tileset->name,
                                            tileset->image->source,
                                            gameContext->renderer,
                                            &gameContext->permanentMemory)) {
      tmx_map_free(map);
      for (tileSetNode = tileSetList; tileSetNode;) {
        TileSet *next = tileSetNode->next;
        freeMemory(&gameContext->longTimeMemory, tileSetNode);
        tileSetNode = next;
      }
      return false;
    }
    TileSet *newTileSetNode = (TileSet *) reserveMemory(&gameContext->longTimeMemory, sizeof(TileSet));
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
    tileSetNode->name = (char *) reserveMemory(&gameContext->longTimeMemory, tileSetNodeNameLen + 1);
    strcpy(tileSetNode->name, tileset->name);
    tileSetNode->numColumns = tileSetNode->imageWidth / (tileSetNode->tileWidth + tileSetNode->spacing);
  }

  size_t sizeofids = map->height * map->width * sizeof(int32_t);
  TileLayer *tileLayerNode = 0;
  for (tmx_layer *tilelayer = map->ly_head; tilelayer; tilelayer = tilelayer->next) {
    if (tilelayer->type == L_LAYER && tilelayer->content.gids) {
      TileLayer *newTileLayerNode = (TileLayer *) reserveMemory(&gameContext->longTimeMemory, 
                                                                sizeof(TileLayer));
      if (!tileMap->tileLayerList) {
        tileMap->tileLayerList = newTileLayerNode;
      }
      if (tileLayerNode) {
        tileLayerNode->next = newTileLayerNode;
      }
      tileLayerNode = newTileLayerNode;
      tileLayerNode->tileGids = (int32_t *) reserveMemory(&gameContext->longTimeMemory, sizeofids);
      memcpy(tileLayerNode->tileGids, tilelayer->content.gids, sizeofids);
      tileLayerNode->tileGidsCount = map->height * map->width;
      tileLayerNode->tileWidth = tileMap->tileWidth;
      tileLayerNode->tileHeight = tileMap->tileHeight;
      tileLayerNode->screenWidth = gameContext->screenWidth;
      tileLayerNode->screenHeight = gameContext->screenWidth;
      tileLayerNode->screenColumns = gameContext->screenWidth / tileMap->tileWidth;
      tileLayerNode->screenRows = gameContext->screenHeight / tileMap->tileHeight;
      tileLayerNode->mapWidth = map->width;
      tileLayerNode->mapHeight = map->height;
      tileLayerNode->tileSetList = tileSetList;
    
    } else if (tilelayer->type == L_OBJGR) {
      tmx_object *object = tilelayer->content.objgr->head;
      int numFrames = 0;
      int textureHeight = 0;
      int textureWidth = 0;
      int callbackId = 0;
      int animSpeed = 0;
      char *textureId = 0;
      for (tmx_property *property = object->properties; property; property = property->next) {
        if (strcmp("numFrames", property->name) == 0) {
          numFrames = atoi(property->value);

        } else if (strcmp("textureHeight", property->name) == 0) {
          textureHeight = atoi(property->value);

        } else if (strcmp("textureWidth", property->name) == 0) {
          textureWidth = atoi(property->value);

        } else if (strcmp("textureId", property->name) == 0) {
          size_t textureIdLen = strlen(property->value);
          textureId = (char *) reserveMemory(&gameContext->longTimeMemory, textureIdLen + 1);
          strcpy(textureId, property->value);

        } else if(strcmp("callbackId", property->name) == 0) {
          callbackId = atoi(property->value);

        } else if(strcmp("animSpeed", property->name) == 0) {
          animSpeed = atoi(property->value);
        }
      }
      SDL_Texture *texture = gameContext->functions.getTexture(textureId);

      Entity *player = (Entity *) reserveMemory(&gameContext->longTimeMemory, sizeof(Entity));
      player->type = PLAYER_TYPE;
      player->position = V2D{(float) object->x, (float) object->y};
      player->bitmap = Bitmap{texture, textureWidth, textureHeight, numFrames, 1, 1};
      player->velocity = V2D{0,0};
      player->acceleration= V2D{0,0};

      tileMap->objectLayer = (ObjectLayer *) reserveMemory(&gameContext->longTimeMemory, sizeof(ObjectLayer));
      tileMap->objectLayer->playerInitialPosition = player->position;
      tileMap->objectLayer->player = player;
    }
  }

  tmx_map_free(map);

  return true;
}
