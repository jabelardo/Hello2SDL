//
// Created by Jose Gutierrez on 10/18/15.
//

#include <libxml/tree.h>
#include <zlib.h>
#include <assert.h>

#include "TileMap.h"
#include "RenderUtils.h"
#include "Entity.h"
#include "TextureStorage.h"
#include "SharedDefinitions.h"
#include "ScrollingBackground.h"

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

TileSet *
getTileSetById(TileLayer *tileLayer, int tileId) {
  for (TileSet *node = tileLayer->tileSetList; node; node = node->next) {
    if (tileId >= node->firstGid && tileId < node->firstGid + node->tileCount) {
      return node;
    }
  }
  return 0;
}

#if 0
void
drawTileLayerTexture(TileLayer *tileLayer, GameContext* gameContext, SDL_Renderer *renderer) {

  int mapWidth = tileLayer->mapWidth * tileLayer->tileWidth;

  if (tileLayer->position.x >= 0) {

    SDL_Rect srcMainRect;
    srcMainRect.x = (int) tileLayer->position.x;
    srcMainRect.y = (int) tileLayer->position.y;

    SDL_Rect destMainRect;
    destMainRect.x = 0;
    destMainRect.y = 0;

    srcMainRect.h = destMainRect.h = gameContext->gameHeight;

    if (tileLayer->position.x < mapWidth - gameContext->gameWidth) {
      srcMainRect.w = destMainRect.w = gameContext->gameWidth;

    } else {

      int mainRectWidth = mapWidth - (int) tileLayer->position.x;

      srcMainRect.w = destMainRect.w = mainRectWidth;

      SDL_Rect srcAuxRect;
      srcAuxRect.x = 0;
      srcAuxRect.y = (int) tileLayer->position.y;

      SDL_Rect destAuxRect;
      destAuxRect.x = mainRectWidth;
      destAuxRect.y = 0;

      srcAuxRect.w = destAuxRect.w = gameContext->gameWidth - destAuxRect.x;
      srcAuxRect.h = destAuxRect.h = gameContext->gameHeight;

      SDL_RenderCopy(renderer, tileLayer->texture, &srcAuxRect, &destAuxRect);
    }

    SDL_RenderCopy(renderer, tileLayer->texture, &srcMainRect, &destMainRect);

  } else {

    SDL_Rect srcMainRect;
    srcMainRect.y = (int) tileLayer->position.y;

    SDL_Rect destMainRect;
    destMainRect.y = 0;

    srcMainRect.h = destMainRect.h = gameContext->gameHeight;

    if (gameContext->gameWidth + tileLayer->position.x < 0) {

      srcMainRect.x = mapWidth + (int) tileLayer->position.x;
      destMainRect.x = 0;
      srcMainRect.w = destMainRect.w = gameContext->gameWidth;

    } else {

      srcMainRect.x = 0;
      destMainRect.x = -1 * (int) tileLayer->position.x;
      srcMainRect.w = destMainRect.w = gameContext->gameWidth - destMainRect.x;

      SDL_Rect srcAuxRect;
      srcAuxRect.x = mapWidth - destMainRect.x;
      srcAuxRect.y = (int) tileLayer->position.y;

      SDL_Rect destAuxRect;
      destAuxRect.x = 0;
      destAuxRect.y = 0;

      srcAuxRect.w = destAuxRect.w = gameContext->gameWidth - srcMainRect.w;
      srcAuxRect.h = destAuxRect.h = gameContext->gameHeight;

      SDL_RenderCopy(renderer, tileLayer->texture, &srcAuxRect, &destAuxRect);
    }

    SDL_RenderCopy(renderer, tileLayer->texture, &srcMainRect, &destMainRect);
  }
}
#endif

#if 0
void
createTileLayerTexture(TileLayer *tileLayer, SDL_Renderer *renderer) {
  tileLayer->texture = SDL_CreateTexture(renderer,
                                         SDL_PIXELFORMAT_UNKNOWN,
                                         SDL_TEXTUREACCESS_TARGET,
                                         tileLayer->mapWidth * tileLayer->tileWidth,
                                         tileLayer->mapHeight * tileLayer->tileHeight);
  SDL_SetRenderTarget(renderer, tileLayer->texture);

  for (int y = 0; y < tileLayer->mapHeight; ++y) {
    for (int x = 0; x < tileLayer->mapWidth; ++x) {
      int tileIdx = y * tileLayer->mapWidth + x;
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
                       currentRow, 2, 2};
      int x1 = x * tileLayer->tileWidth;
      int y1 = y * tileLayer->tileHeight;
      drawTile(renderer, x1, y1, &bitmap);
    }
  }
  SDL_SetRenderTarget(renderer, 0);
}
#endif

#if 0
void
drawTileLayers(TileLayer *tileLayer, V2D cameraPosition, SDL_Renderer *renderer) {
  int relativeLayerX = (int) (tileLayer->position.x - cameraPosition.x);
  int relativeLayerY = (int) (tileLayer->position.y - cameraPosition.y);

  int relativeLayerColumn = relativeLayerX / tileLayer->tileWidth;
  int relativeLayerRow = relativeLayerY / tileLayer->tileHeight;

  int startColumn = (int) tileLayer->position.x / tileLayer->tileWidth;
  int startRow = (int) tileLayer->position.y / tileLayer->tileHeight;

  int adjustX = -relativeLayerX % tileLayer->tileWidth;
  int adjustY = -relativeLayerY % tileLayer->tileHeight;

  for (int screenRow = (adjustY < 0 ? -1 : 0);
       screenRow < (adjustY <= 0 ? tileLayer->screenRows : tileLayer->screenRows + 1);
       ++screenRow) {
    for (int screenColumn = (adjustX < 0 ? -1 : 0);
         screenColumn < (adjustX <= 0 ? tileLayer->screenColumns : tileLayer->screenColumns + 1);
         ++screenColumn) {
      int tileColumn = startColumn - relativeLayerColumn + screenColumn;
      if (tileColumn >= tileLayer->mapWidth) {
        tileColumn -= tileLayer->mapWidth;
      } else if (tileColumn < 0) {
        tileColumn += tileLayer->mapWidth;
      }
      int tileRow = startRow - relativeLayerRow + screenRow;
      if (tileRow >= tileLayer->mapHeight) {
        tileRow -= tileLayer->mapHeight;
      } else if (tileRow < 0) {
        tileRow += tileLayer->mapHeight;
      }
      int tileIdx = tileRow * tileLayer->mapWidth + tileColumn;
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
                       currentRow, 2, 2};
      int x = (screenColumn * tileLayer->tileWidth) - adjustX;
      int y = (screenRow * tileLayer->tileHeight) - adjustY;
      drawTile(renderer, x, y, &bitmap);
    }
  }
}
#endif

#if 0
void
updateTileLayer(TileLayer *tileLayer, GameContext *gameContext) {
#if 0
  tileLayer->velocity.x = gameContext->scrollSpeed;
  tileLayer->position += tileLayer->velocity;
  if (tileLayer->position.x >= tileLayer->mapWidth * tileLayer->tileWidth ||
      tileLayer->position.x <= -tileLayer->mapWidth * tileLayer->tileWidth) {
    tileLayer->position.x = 0;
  }
  if (tileLayer->position.y >= tileLayer->mapHeight * tileLayer->tileHeight ||
      tileLayer->position.y <= -tileLayer->mapHeight * tileLayer->tileHeight) {
    tileLayer->position.y = 0;
  }
#else
  if (tileLayer->position.x <
      (tileLayer->mapWidth * tileLayer->tileWidth) - gameContext->gameWidth - tileLayer->tileWidth) {
    tileLayer->velocity.x = gameContext->scrollSpeed;
    tileLayer->position += tileLayer->velocity;
  } else {
    tileLayer->velocity.x = 0;
  }
#endif
}
#endif

char *
stringTrim(char *str) {
  char *pos = str;
  while (*pos++) {
    if (*pos == '\n' || *pos == '\r') {
      *pos = ' ';
    }
  }
  // Trim leading space
  while (isspace(*str)) {
    ++str;
  }
  if (*str == 0) {
    return str;
  }
  // Trim trailing space
  char *end = str + strlen(str) - 1;
  while (end > str && isspace(*end)) {
    --end;
  }
  // Write new null terminator
  *(end + 1) = 0;
  return str;
}

char
b64Value(char c) {
  if (c >= 'A' && c <= 'Z') {
    return (char) c - 'A';
  } else if (c >= 'a' && c <= 'z') {
    return (char) (c - 'a' + 26);
  } else if (c >= '0' && c <= '9') {
    return (char) (c - '0' + 52);
  } else if (c == '+') {
    return (char) 62;
  } else if (c == '/') {
    return (char) 63;
  } else if (c == '=') {
    return (char) 0;
  }
  return (char) -1;
}

char *
b64Decode(char *source, size_t *resultLen, MemoryPartition *memoryPartition) {
  if (!source) {
    return 0;
  }

  size_t sourceLen = strlen(source);
  if (sourceLen % 4) {
    return 0; /* invalid source */
  }

  *resultLen = (sourceLen / 4) * 3;
  char *result = (char *) reserveMemory(memoryPartition, *resultLen);
  if (!result) {
    return 0;
  }

  for (size_t i = 0; i < sourceLen; i += 4) {
    size_t in = 0;

    for (int j = 0; j < 4; ++j) {
      char b64Char = b64Value(source[i + j]);
      if (b64Char == -1) {
        goto fail;
      }
      in = in << 6;
      in += b64Char; /* add 6b */
    }
    for (int j = 0; j < 3; ++j) {
      /* copy 3 bytes in reverse order */
      memcpy(result + (i / 4) * 3 + j, ((char *) &in) + 2 - j, 1);
    }
  }

  if (source[sourceLen - 1] == '=') {
    (*resultLen)--;
  }
  if (source[sourceLen - 2] == '=') {
    (*resultLen)--;
  }

  return result;

  fail:
  freeMemory(memoryPartition, result);
  return 0;
}

alloc_func
zlibAllocFunction(MemoryPartition *memoryPartition) {
  static MemoryPartition *partition = memoryPartition;
  alloc_func result = [](void *opaque, unsigned int items, unsigned int size) {
    return reserveMemory(partition, items * size);
  };
  return result;
}

free_func
zlibFreeFunction(MemoryPartition *memoryPartition) {
  static MemoryPartition *partition = memoryPartition;
  free_func result = [](void *opaque, void *address) {
    freeMemory(partition, address);
  };
  return result;
}

int
zlibDecompress(Bytef *dest, size_t *destLen, const Bytef *source, size_t sourceLen,
               MemoryPartition *memoryPartition) {

  if (!dest || !destLen || !source || !sourceLen) {
    return Z_DATA_ERROR;
  }

  z_stream stream;
  stream.next_in = (Bytef *) source;
  stream.avail_in = (uInt) sourceLen;

  stream.next_out = dest;
  stream.avail_out = (uInt) *destLen;

  /* Check for source > 64K on 16-bit machine: */
  if ((uLong) stream.avail_in != sourceLen) {
    return Z_BUF_ERROR;
  }
  if ((uLong) stream.avail_out != *destLen) {
    return Z_BUF_ERROR;
  }

  stream.zalloc = zlibAllocFunction(memoryPartition);
  stream.zfree = zlibFreeFunction(memoryPartition);
  stream.opaque = Z_NULL;

  /* 15+32 to enable zlib and gzip decoding with automatic header detection */
  int retCode = inflateInit2(&stream, 15 + 32);

  if (retCode != Z_OK) {
    return retCode;
  }

  retCode = inflate(&stream, Z_FINISH);

  if (retCode != Z_STREAM_END) {
    inflateEnd(&stream);
    if (retCode == Z_NEED_DICT || (retCode == Z_BUF_ERROR && stream.avail_in == 0)) {
      return Z_DATA_ERROR;
    }
    return retCode;
  }
  *destLen = stream.total_out;

  retCode = inflateEnd(&stream);

  return retCode;
}

bool
dataDecode(char *source, size_t gidsCount, int32_t **gids, MemoryPartition *memoryPartition) {

  size_t b64DecodedLen = 0;
  char *b64Decoded = b64Decode(source, &b64DecodedLen, memoryPartition);
  if (!b64Decoded) {
    return false;
  }
  size_t gidsSize = gidsCount * sizeof(int32_t);
  *gids = (int32_t *) reserveMemory(memoryPartition, gidsSize);

  switch (zlibDecompress((Bytef *) *gids, &gidsSize, (const Bytef *) b64Decoded, b64DecodedLen,
                         memoryPartition)) {
    case Z_OK: {
      // Z_OK if success
      break;
    }
    case Z_MEM_ERROR: {
      // Z_MEM_ERROR if there was not enough memory
      goto fail;
    }
    case Z_BUF_ERROR: {
      // Z_BUF_ERROR if there was not enough room in the output buffer
      goto fail;
    }
    case Z_DATA_ERROR: {
      // Z_DATA_ERROR if the input data was corrupted or incomplete.
      goto fail;
    }
  }
  return *gids != 0;

  fail:
  freeMemory(memoryPartition, *gids);
  return false;
}

xmlFreeFunc
xmlFreeFunction(MemoryPartition *memoryPartition) {
  static MemoryPartition *partition = memoryPartition;
  xmlFreeFunc result = [](void *mem) {
  };
  return result;
}

xmlMallocFunc
xmlMallocFunction(MemoryPartition *memoryPartition) {
  static MemoryPartition *partition = memoryPartition;
  xmlMallocFunc result = [](size_t size) {
    return reserveMemory(partition, size);
  };
  return result;
}

xmlReallocFunc
xmlReallocFunction(MemoryPartition *memoryPartition) {
  static MemoryPartition *partition = memoryPartition;
  xmlReallocFunc result = [](void *mem, size_t size) {
    freeMemory(partition, mem);
    return reserveMemory(partition, size);
  };
  return result;
}

xmlStrdupFunc
xmlStrdupFunction(MemoryPartition *memoryPartition) {
  static MemoryPartition *partition = memoryPartition;
  xmlStrdupFunc result = [](const char *str) -> char * {
    if (!str) {
      return 0;
    }
    size_t size = strlen(str) + 1;
    void *mem = reserveMemory(partition, size);
    if (!mem) {
      return 0;
    }
    memcpy(mem, str, size);
    return (char *) mem;
  };
  return result;
}

xmlNode *
getXmlElement(xmlNode *node, const xmlChar *name) {
  for (xmlNode *curNode = node; curNode; curNode = curNode->next) {
    if (curNode->type == XML_ELEMENT_NODE && xmlStrcmp(curNode->name, name) == 0) {
      return curNode;
    }
  }
  return node->children ? getXmlElement(node->children, name) : 0;
}

bool
xmlGetProp(xmlNode *node, const xmlChar *name, int *value) {
  xmlChar *prop = xmlGetProp(node, name);
  if (!prop) {
    return false;
  }
  *value = atoi((const char *) prop);
  return true;
}

EntityNode *
createTileLayerEntities(TileLayer *tileLayer, MemoryPartition *memoryPartition) {

  EntityNode *resultList = 0;

  for (int y = 0; y < tileLayer->mapHeight; ++y) {
    for (int x = 0; x < tileLayer->mapWidth; ++x) {
      int tileIdx = y * tileLayer->mapWidth + x;
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
      int objectX = x * tileLayer->tileWidth;
      int objectY = y * tileLayer->tileHeight;

      EntityNode *newEntityNode = RESERVE_MEMORY(memoryPartition, EntityNode);
      newEntityNode->entity.health = 1;
      newEntityNode->entity.type = TILE_TYPE;
      newEntityNode->entity.position = {ceilf((float) objectX) + ((float) tileLayer->tileWidth) / 2.f,
                                        ceilf((float) objectY) + ((float) tileLayer->tileHeight) / 2.f};
      newEntityNode->entity.bitmap = {texture, tileLayer->tileWidth, tileLayer->tileHeight, 0, currentFrame, currentRow,
                                      tileSet->margin, tileSet->spacing};

      newEntityNode->entity.halfCollisionDim = ((float) tileLayer->tileWidth) / 2.f;

      if (!tileLayer->collidable) {
        setEntityFlags(&newEntityNode->entity, DONT_COLLIDE_FLAG);
      }
      newEntityNode->next = resultList;
      resultList = newEntityNode;
    }
  }
  return resultList;
}

bool
loadTileMap(TileMap *tileMap, const char *mapfileName, GameContext *gameContext,
            SDL_Renderer *renderer, GameMemory *gameMemory, PlatformConfig *platformConfig) {

  assert(tileMap->entityList == 0);

  if (xmlMemSetup(xmlFreeFunction(&gameMemory->shortTimeMemory),
                  xmlMallocFunction(&gameMemory->shortTimeMemory),
                  xmlReallocFunction(&gameMemory->shortTimeMemory),
                  xmlStrdupFunction(&gameMemory->shortTimeMemory))) {
    return false;
  }
  xmlInitParser();

  char *mapfilePath = stringConcat(platformConfig->resourcePath, mapfileName,
                                   &gameMemory->shortTimeMemory);

  if (!mapfilePath) {
    return false;
  }

  xmlDoc *doc = xmlReadFile(mapfilePath, 0, 0);

  TileSet *tileSetList = 0;
  TileLayer *tileLayerList = 0;
  if (!doc) {
    goto fail;
  }
  {
    xmlNode *root = xmlDocGetRootElement(doc);
    if (!root) {
      goto fail;
    }

    xmlNode *map = getXmlElement(root, (const xmlChar *) "map");
    if (!map) {
      goto fail;
    }

    if (!xmlGetProp(map, (const xmlChar *) "width", &tileMap->width)) {
      goto fail;
    }
    if (!xmlGetProp(map, (const xmlChar *) "height", &tileMap->height)) {
      goto fail;
    }
    if (!xmlGetProp(map, (const xmlChar *) "tilewidth", &tileMap->tileWidth)) {
      goto fail;
    }
    if (!xmlGetProp(map, (const xmlChar *) "tileheight", &tileMap->tileHeight)) {
      goto fail;
    }

    xmlNode *mapProperties = getXmlElement(map, (const xmlChar *) "properties");
    if (!mapProperties) {
      goto fail;
    }
    for (xmlNode *property = mapProperties->children; property; property = property->next) {
      if (property->type == XML_ELEMENT_NODE &&
          xmlStrcmp(property->name, (const xmlChar *) "property") == 0) {
        char *name = (char *) xmlGetProp(property, (const xmlChar *) "name");
        if (!name) {
          goto fail;
        }
        char *value = (char *) xmlGetProp(property, (const xmlChar *) "value");
        if (!value) {
          goto fail;
        }
        if (!loadTexture(name, value, platformConfig->resourcePath, renderer, gameContext,
                         gameMemory)) {
          goto fail;
        }
      }
    }

    for (xmlNode *tileset = map->children; tileset; tileset = tileset->next) {
      if (tileset->type == XML_ELEMENT_NODE &&
          xmlStrcmp(tileset->name, (const xmlChar *) "tileset") == 0) {
        TileSet *newTileSet = RESERVE_MEMORY(&gameMemory->shortTimeMemory, TileSet);
        newTileSet->next = tileSetList;
        tileSetList = newTileSet;
        if (!xmlGetProp(tileset, (const xmlChar *) "firstgid", &newTileSet->firstGid)) {
          goto fail;
        }
        if (!xmlGetProp(tileset, (const xmlChar *) "tilewidth", &newTileSet->tileWidth)) {
          goto fail;
        }
        if (!xmlGetProp(tileset, (const xmlChar *) "tileheight", &newTileSet->tileHeight)) {
          goto fail;
        }
        if (!xmlGetProp(tileset, (const xmlChar *) "spacing", &newTileSet->spacing)) {
          goto fail;
        }
        if (!xmlGetProp(tileset, (const xmlChar *) "margin", &newTileSet->margin)) {
          goto fail;
        }
        if (!xmlGetProp(tileset, (const xmlChar *) "tilecount", &newTileSet->tileCount)) {
          goto fail;
        }
        char *name = (char *) xmlGetProp(tileset, (const xmlChar *) "name");
        if (!name) {
          goto fail;
        }
        newTileSet->name = name;

        for (xmlNode *image = tileset->children; image; image = image->next) {
          if (image->type == XML_ELEMENT_NODE &&
              xmlStrcmp(image->name, (const xmlChar *) "image") == 0) {
            if (!xmlGetProp(image, (const xmlChar *) "width", &newTileSet->imageWidth)) {
              goto fail;
            }
            if (!xmlGetProp(image, (const xmlChar *) "height", &newTileSet->imageHeight)) {
              goto fail;
            }
            char *source = (char *) xmlGetProp(image, (const xmlChar *) "source");
            if (!source) {
              goto fail;
            }
            if (!loadTexture(newTileSet->name, source, platformConfig->resourcePath, renderer,
                             gameContext, gameMemory)) {
              goto fail;
            }
            newTileSet->texture = getTexture(newTileSet->name, gameContext);
          }
        }
        newTileSet->numColumns =
            newTileSet->imageWidth / (newTileSet->tileWidth + newTileSet->spacing);
      }
    }

    for (xmlNode *layer = map->children; layer; layer = layer->next) {
      if (layer->type == XML_ELEMENT_NODE &&
          xmlStrcmp(layer->name, (const xmlChar *) "layer") == 0) {

        bool collidable = false;

        xmlNode *layerProperties = getXmlElement(layer, (const xmlChar *) "properties");
        if (layerProperties) {
          for (xmlNode *property = layerProperties->children; property; property = property->next) {
            if (property->type == XML_ELEMENT_NODE &&
                xmlStrcmp(property->name, (const xmlChar *) "property") == 0) {
              char *name = (char *) xmlGetProp(property, (const xmlChar *) "name");
              if (!name) {
                goto fail;
              }
              char *value = (char *) xmlGetProp(property, (const xmlChar *) "value");
              if (!value) {
                goto fail;
              }
              if ((strcmp(name, "collidable")) == 0 && (strcmp(value, "true")) == 0) {
                collidable = true;
              }
            }
          }
        }
        
        xmlNode *data = getXmlElement(layer, (const xmlChar *) "data");
        if (!data) {
          goto fail;
        }
        TileLayer *newTileLayer = RESERVE_MEMORY(&gameMemory->shortTimeMemory, TileLayer);
        newTileLayer->next = tileLayerList;
        tileLayerList = newTileLayer;

        newTileLayer->tileWidth = tileMap->tileWidth;
        newTileLayer->tileHeight = tileMap->tileHeight;
        newTileLayer->screenColumns = platformConfig->screenWidth / tileMap->tileWidth;
        newTileLayer->screenRows = platformConfig->screenHeight / tileMap->tileHeight;
        newTileLayer->mapWidth = tileMap->width;
        newTileLayer->mapHeight = tileMap->height;
        newTileLayer->tileSetList = tileSetList;
        newTileLayer->tileGidsCount = (size_t) (tileMap->height * tileMap->width);
        newTileLayer->collidable = collidable;

        xmlChar *base64Gids = xmlNodeGetContent(data);
        char *trimBase64Gids = stringTrim((char *) base64Gids);
        int32_t *gids = 0;
        if (!dataDecode(trimBase64Gids, newTileLayer->tileGidsCount, &gids,
                        &gameMemory->shortTimeMemory)) {
          goto fail;
        }
        newTileLayer->tileGids = gids;

        EntityNode *entityList = createTileLayerEntities(newTileLayer, &gameMemory->longTimeMemory);
        if (tileMap->entityList) {
          EntityNode* node = tileMap->entityList;
          for (; node->next ; node = node->next) {
            // skip
          }
          node->next = entityList;

        } else {
          tileMap->entityList = entityList;
        }
      }
    }

    for (xmlNode *objectgroup = map->children; objectgroup; objectgroup = objectgroup->next) {
      if (objectgroup->type == XML_ELEMENT_NODE &&
          xmlStrcmp(objectgroup->name, (const xmlChar *) "objectgroup") == 0) {

        for (xmlNode *object = objectgroup->children; object; object = object->next) {
          if (object->type == XML_ELEMENT_NODE &&
              xmlStrcmp(object->name, (const xmlChar *) "object") == 0) {

            int objectX = 0;
            int objectY = 0;
            if (!xmlGetProp(object, (const xmlChar *) "x", &objectX)) {
              goto fail;
            }
            if (!xmlGetProp(object, (const xmlChar *) "y", &objectY)) {
              goto fail;
            }
            char *type = (char *) xmlGetProp(object, (const xmlChar *) "type");
            if (!type) {
              goto fail;
            }
            xmlNode *objectProperties = getXmlElement(object, (const xmlChar *) "properties");
            if (!objectProperties) {
              goto fail;
            }
            int numFrames = 0;
            int textureHeight = 0;
            int textureWidth = 0;
            float animSpeed = 0;
            char *textureId = 0;
            for (xmlNode *property = objectProperties->children; property; property = property->next) {
              if (property->type == XML_ELEMENT_NODE &&
                  xmlStrcmp(property->name, (const xmlChar *) "property") == 0) {

                char *propertyName = (char *) xmlGetProp(property, (const xmlChar *) "name");
                if (!propertyName) {
                  goto fail;
                }
                char *propertyValue = (char *) xmlGetProp(property, (const xmlChar *) "value");
                if (!propertyValue) {
                  goto fail;

                } else if (strcmp(propertyName, "numFrames") == 0) {
                  numFrames = atoi(propertyValue);

                } else if (strcmp(propertyName, "animSpeed") == 0) {
                  animSpeed = (float) atof(propertyValue);

                } else if (strcmp(propertyName, "textureHeight") == 0) {
                  textureHeight = atoi(propertyValue);

                } else if (strcmp(propertyName, "textureWidth") == 0) {
                  textureWidth = atoi(propertyValue);

                } else if (strcmp(propertyName, "textureId") == 0) {
                  size_t propertyValueLen = strlen(propertyValue);
                  textureId = (char *) reserveMemory(&gameMemory->shortTimeMemory,
                                                     propertyValueLen + 1);
                  memcpy(textureId, propertyValue, propertyValueLen + 1);
                }
              }
            }
            if (strcmp(type, "ScrollingBackground") == 0) {
              SDL_Texture *texture = getTexture(textureId, gameContext);
              tileMap->scrollingBackground = RESERVE_MEMORY(&gameMemory->longTimeMemory,
                                                            ScrollingBackground);

              *tileMap->scrollingBackground = {};
              tileMap->scrollingBackground->position = {(float) objectX, (float) objectY};
              tileMap->scrollingBackground->animSpeed = animSpeed;
              tileMap->scrollingBackground->bitmap = {texture, textureWidth, textureHeight,
                                                      numFrames};

            } else {
              EntityType entityType = parseEntityType(type);
              SDL_Texture *texture = getTexture(textureId, gameContext);
              EntityNode *newEntityNode = RESERVE_MEMORY(&gameMemory->longTimeMemory, EntityNode);
              newEntityNode->entity.type = entityType;
              newEntityNode->entity.position = {ceilf((float) objectX) + ((float) textureWidth) / 2.f,
                                        ceilf((float) objectY) + ((float) textureHeight) / 2.f};
              newEntityNode->entity.bitmap = {texture, textureWidth, textureHeight, numFrames};
              newEntityNode->entity.velocity = {0, 0};
              newEntityNode->next = tileMap->entityList;
              tileMap->entityList = newEntityNode;
            }
          }
        }
      }
    }

    xmlCleanupParser();

    gameMemory->shortTimeMemory.usedSize = 0;

    return true;
  }
  fail:
  if (tileMap->scrollingBackground) {
    freeMemory(&gameMemory->longTimeMemory, tileMap->scrollingBackground);
    tileMap->scrollingBackground = 0;
  }
  for (EntityNode *entityNode = tileMap->entityList; entityNode; entityNode = entityNode->next) {
    freeMemory(&gameMemory->longTimeMemory, entityNode);
  }
  tileMap->entityList = 0;
  gameMemory->shortTimeMemory.usedSize = 0;
  return false;
}
