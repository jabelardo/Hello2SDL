//
// Created by Jose Gutierrez on 10/18/15.
//

#include <libxml/tree.h>
#include "TileMap.h"
#include "RenderUtils.h"
#include "Entity.h"
#include "XmlUtils.h"

TileSet *
getTileSetById(TileLayer *tileLayer, int tileId) {
  for (TileSet *node = tileLayer->tileSetList; node; node = node->next) {
    if (tileId >= node->firstGid && tileId < node->firstGid + node->tileCount) {
      return node;
    }
  }
  return 0;
}

void
drawTileLayer(TileLayer *tileLayer, SDL_Renderer *renderer) {
  int x = (int) tileLayer->position.x / tileLayer->tileWidth;
  int y = (int) tileLayer->position.y / tileLayer->tileHeight;
  int x2 = (int) tileLayer->position.x % tileLayer->tileWidth;
  int y2 = (int) tileLayer->position.y % tileLayer->tileHeight;
  for (int i = (y2 < 0 ? -1 : 0);
       i < (y2 <= 0 ? tileLayer->screenRows : tileLayer->screenRows + 1); ++i) {
    for (int j = (x2 < 0 ? -1 : 0);
         j < (x2 <= 0 ? tileLayer->screenColumns : tileLayer->screenColumns + 1); ++j) {
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
      Bitmap bitmap = {texture, tileLayer->tileWidth, tileLayer->tileHeight, 0, currentFrame,
                       currentRow};
      int x3 = (j * tileLayer->tileWidth) - x2;
      int y3 = (i * tileLayer->tileHeight) - y2;
      drawTile(renderer, 2, 2, x3, y3, &bitmap);
    }
  }
}

void
drawObjectLayer(ObjectLayer *objectLayer, SDL_Renderer *renderer) {
  drawEntity(objectLayer->player, renderer);
}

void
drawTileMap(TileMap *tileMap, SDL_Renderer *renderer) {
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
updateObjectLayer(ObjectLayer *objectLayer, GameContext *gameContext) {
  updateEntity(objectLayer->player, gameContext->userInput);
}

void
updateTileMap(TileMap *tileMap, GameContext *gameContext) {
  for (TileLayer *node = tileMap->tileLayerList; node; node = node->next) {
    updateTileLayer(node, gameContext);
  }
  updateObjectLayer(tileMap->objectLayer, gameContext);
}

bool
initTileMap(TileMap *tileMap, GameContext *gameContext, const char *mapfile) {

  xmlDoc *doc = xmlReadFile(mapfile, 0, 0);
  if (!doc) {
    return false;
  }

  xmlNode *root = xmlDocGetRootElement(doc);
  if (!root) {
    return false;
  }

  xmlNode *map = getXmlElement(root, (const xmlChar *) "map");
  if (!map) {
    return false;
  }

  if (!xmlGetProp(map, (const xmlChar *) "width", &tileMap->width)) {
    return false;
  }
  if (!xmlGetProp(map, (const xmlChar *) "height", &tileMap->height)) {
    return false;
  }
  if (!xmlGetProp(map, (const xmlChar *) "tilewidth", &tileMap->tileWidth)) {
    return false;
  }
  if (!xmlGetProp(map, (const xmlChar *) "tileheight", &tileMap->tileHeight)) {
    return false;
  }
  tileMap->tileLayerList = 0;

  xmlNode *mapProperties = getXmlElement(map, (const xmlChar *) "properties");
  if (!mapProperties) {
    return false;
  }
  for (xmlNode *property = mapProperties->children; property; property = property->next) {
    if (property->type == XML_ELEMENT_NODE &&
        xmlStrcmp(property->name, (const xmlChar *) "property") == 0) {
      char *name = (char *) xmlGetProp(property, (const xmlChar *) "name");
      char *value = (char *) xmlGetProp(property, (const xmlChar *) "value");
      if (!gameContext->functions.loadTexture(name, value, gameContext->renderer,
                                              &gameContext->permanentMemory)) {
        return false;
      }
    }
  }

  TileSet *tileSetList = 0;
  TileSet *tileSetNode = 0;
  for (xmlNode *tileset = map->children; tileset; tileset = tileset->next) {
    if (tileset->type == XML_ELEMENT_NODE &&
        xmlStrcmp(tileset->name, (const xmlChar *) "tileset") == 0) {
      TileSet *newTileSet = (TileSet *) reserveMemory(&gameContext->longTimeMemory,
                                                      sizeof(TileSet));
      if (!tileSetList) {
        tileSetList = newTileSet;
      }
      if (tileSetNode) {
        tileSetNode->next = newTileSet;
      }
      tileSetNode = newTileSet;
      if (!xmlGetProp(tileset, (const xmlChar *) "firstgid", &newTileSet->firstGid)) {
        return false;
      }
      if (!xmlGetProp(tileset, (const xmlChar *) "tilewidth", &newTileSet->tileWidth)) {
        return false;
      }
      if (!xmlGetProp(tileset, (const xmlChar *) "tileheight", &newTileSet->tileHeight)) {
        return false;
      }
      if (!xmlGetProp(tileset, (const xmlChar *) "spacing", &newTileSet->spacing)) {
        return false;
      }
      if (!xmlGetProp(tileset, (const xmlChar *) "margin", &newTileSet->margin)) {
        return false;
      }
      if (!xmlGetProp(tileset, (const xmlChar *) "tilecount", &newTileSet->tileCount)) {
        return false;
      }
      char *name = (char *) xmlGetProp(tileset, (const xmlChar *) "name");
      size_t tileSetNodeNameLen = strlen(name);
      newTileSet->name = (char *) reserveMemory(&gameContext->longTimeMemory,
                                                tileSetNodeNameLen + 1);
      strcpy(newTileSet->name, name);

      for (xmlNode *image = tileset->children; image; image = image->next) {
        if (image->type == XML_ELEMENT_NODE &&
            xmlStrcmp(image->name, (const xmlChar *) "image") == 0) {
          if (!xmlGetProp(image, (const xmlChar *) "width", &newTileSet->imageWidth)) {
            return false;
          }
          if (!xmlGetProp(image, (const xmlChar *) "height", &newTileSet->imageHeight)) {
            return false;
          }
          char *source = (char *) xmlGetProp(image, (const xmlChar *) "source");
          if (!gameContext->functions.loadTexture(newTileSet->name, source, gameContext->renderer,
                                                  &gameContext->permanentMemory)) {
            return false;
          }
          newTileSet->texture = gameContext->functions.getTexture(newTileSet->name);
        }
      }
      newTileSet->numColumns =
          newTileSet->imageWidth / (newTileSet->tileWidth + newTileSet->spacing);
    }
  }

  size_t sizeofids = tileMap->height * tileMap->width * sizeof(int32_t);
  TileLayer *tileLayerNode = 0;
//  for (tmx_layer *tilelayer = map->ly_head; tilelayer; tilelayer = tilelayer->next) {
//    if (tilelayer->type == L_LAYER && tilelayer->content.gids) {
//      TileLayer *newTileLayerNode = (TileLayer *) reserveMemory(&gameContext->longTimeMemory,
//                                                                sizeof(TileLayer));
//      if (!tileMap->tileLayerList) {
//        tileMap->tileLayerList = newTileLayerNode;
//      }
//      if (tileLayerNode) {
//        tileLayerNode->next = newTileLayerNode;
//      }
//      tileLayerNode = newTileLayerNode;
//      tileLayerNode->tileGids = (int32_t *) reserveMemory(&gameContext->longTimeMemory, sizeofids);
//      memcpy(tileLayerNode->tileGids, tilelayer->content.gids, sizeofids);
//      tileLayerNode->tileGidsCount = map->height * map->width;
//      tileLayerNode->tileWidth = tileMap->tileWidth;
//      tileLayerNode->tileHeight = tileMap->tileHeight;
//      tileLayerNode->screenWidth = gameContext->screenWidth;
//      tileLayerNode->screenHeight = gameContext->screenWidth;
//      tileLayerNode->screenColumns = gameContext->screenWidth / tileMap->tileWidth;
//      tileLayerNode->screenRows = gameContext->screenHeight / tileMap->tileHeight;
//      tileLayerNode->mapWidth = map->width;
//      tileLayerNode->mapHeight = map->height;
//      tileLayerNode->tileSetList = tileSetList;
//  }


  xmlNode *objectgroup = getXmlElement(map, (const xmlChar *) "objectgroup");
  if (!objectgroup) {
    return false;
  }
  xmlNode *object = getXmlElement(objectgroup, (const xmlChar *) "object");
  if (!object) {
    return false;
  }

  int objectX = 0;
  int objectY = 0;
  if (!xmlGetProp(object, (const xmlChar *) "x", &objectX)) {
    return false;
  }
  if (!xmlGetProp(object, (const xmlChar *) "y", &objectY)) {
    return false;
  }

  xmlNode *objectProperties = getXmlElement(object, (const xmlChar *) "properties");
  if (!objectProperties) {
    return false;
  }
  for (xmlNode *property = objectProperties->children; property; property = property->next) {
    if (property->type == XML_ELEMENT_NODE &&
        xmlStrcmp(property->name, (const xmlChar *) "property") == 0) {
      int numFrames = 0;
      int textureHeight = 0;
      int textureWidth = 0;
      int callbackId = 0;
      int animSpeed = 0;
      if (!xmlGetProp(property, (const xmlChar *) "numFrames", &numFrames)) {
        return false;
      }
      if (!xmlGetProp(property, (const xmlChar *) "textureHeight", &textureHeight)) {
        return false;
      }
      if (!xmlGetProp(property, (const xmlChar *) "textureWidth", &textureWidth)) {
        return false;
      }
      if (!xmlGetProp(property, (const xmlChar *) "callbackId", &callbackId)) {
        return false;
      }
      if (!xmlGetProp(property, (const xmlChar *) "animSpeed", &animSpeed)) {
        return false;
      }
      char *textureId = (char *) xmlGetProp(property, (const xmlChar *) "textureId");

      SDL_Texture *texture = gameContext->functions.getTexture(textureId);

      Entity *player = (Entity *) reserveMemory(&gameContext->longTimeMemory, sizeof(Entity));
      player->type = PLAYER_TYPE;
      player->position = V2D{(float) objectX, (float) objectY};
      player->bitmap = Bitmap{texture, textureWidth, textureHeight, numFrames, 1, 1};
      player->velocity = V2D{0,0};
      player->acceleration= V2D{0,0};

      tileMap->objectLayer = (ObjectLayer *) reserveMemory(&gameContext->longTimeMemory, sizeof(ObjectLayer));
      tileMap->objectLayer->playerInitialPosition = player->position;
      tileMap->objectLayer->player = player;
    }
  }
  return true;
}
