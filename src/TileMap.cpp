//
// Created by Jose Gutierrez on 10/18/15.
//

#include "TileMap.h"
#include "tmx.h"
#include "RenderUtils.h"

#include "tmx.h"

void
TileLayer::draw(SDL_Renderer *renderer) {
  int x = (int) position.x / tileWidth;
  int y = (int) position.y / tileHeight;
  int x2 = (int) position.x % tileWidth;
  int y2 = (int) position.y % tileHeight;
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
      SDL_Texture *texture = tileSet->texture;
      int currentFrame = (tileId - 1 - (tileSet->firstGid - 1)) % tileSet->numColumns;
      int currentRow = (tileId - 1 - (tileSet->firstGid - 1)) / tileSet->numColumns;
      Bitmap bitmap = {texture, tileWidth, tileHeight, 0, currentFrame, currentRow};
      int x3 = (j * tileWidth) - x2;
      int y3 = (i * tileHeight) - y2;
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

  tmx_map *map = tmx_load(mapfile);
  if (!map) {
    return false;
  }

  width = map->width;
  height = map->height;
  tileWidth = map->tile_width;
  tileHeight = map->tile_height;
  tileLayerList = 0;

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
      tmx_object *object = tilelayer->content.objgr->head;
      objectLayer = (ObjectLayer *) reserveMemory(&gameContext->longTimeMemory, sizeof(ObjectLayer));
      Player *player = (Player *) reserveMemory(&gameContext->longTimeMemory, sizeof(Player));
      objectLayer->player = player;
      player->entity.position = V2D{(float) object->x, (float) object->y};
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
