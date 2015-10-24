//
// Created by Jose Gutierrez on 10/18/15.
//

#include <libxml/tree.h>
#include <zlib.h>

#include "TileMap.h"
#include "RenderUtils.h"
#include "Entity.h"

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
    str++;
  }
  if (*str == 0) {
    return str;
  }
  // Trim trailing space
  char *end = str + strlen(str) - 1;
  while (end > str && isspace(*end)) {
    end--;
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
zlibAllocFunction(MemoryPartition* memoryPartition) {
  static MemoryPartition* partition = memoryPartition;
  alloc_func result = [](void *opaque, unsigned int items, unsigned int size) {
    return reserveMemory(partition, items * size);
  };
  return result;
}

free_func
zlibFreeFunction(MemoryPartition* memoryPartition) {
  static MemoryPartition* partition = memoryPartition;
  free_func result = [](void *opaque, void *address) {
    freeMemory(partition, address);
  };
  return result;
}

int
zlibDecompress(Bytef *dest, size_t *destLen, const Bytef *source, size_t sourceLen,
               MemoryPartition *memoryPartition) {

  if (!dest || ! destLen || !source || !sourceLen) {
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
xmlFreeFunction(MemoryPartition* memoryPartition) {
  static MemoryPartition* partition = memoryPartition;
  xmlFreeFunc result = [](void *mem) {
        freeMemory(partition, mem);
  };
  return result;
}

xmlMallocFunc
xmlMallocFunction(MemoryPartition* memoryPartition) {
  static MemoryPartition* partition = memoryPartition;
  xmlMallocFunc result = [](size_t size) {
    return reserveMemory(partition, size);
  };
  return result;
}

xmlReallocFunc
xmlReallocFunction(MemoryPartition* memoryPartition) {
  static MemoryPartition* partition = memoryPartition;
  xmlReallocFunc result = [](void *mem, size_t size) {
    freeMemory(partition, mem);
    return reserveMemory(partition, size);
  };
  return result;
}

xmlStrdupFunc
xmlStrdupFunction(MemoryPartition* memoryPartition) {
  static MemoryPartition* partition = memoryPartition;
  xmlStrdupFunc result = [](const char *str) -> char* {
    if (!str) {
      return 0;
    }
    size_t size = strlen(str) + 1;
    void* mem = reserveMemory(partition, size);
    if (!mem) {
      return 0;
    }
    memcpy(mem, str, size);
    return (char*) mem;
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
  xmlFree(prop);
  return true;
}

bool
initTileMap(TileMap *tileMap, GameContext *gameContext, const char *mapfile) {

  if (xmlMemSetup(xmlFreeFunction(&gameContext->shortTimetMemory),
                  xmlMallocFunction(&gameContext->shortTimetMemory),
                  xmlReallocFunction(&gameContext->shortTimetMemory),
                  xmlStrdupFunction(&gameContext->shortTimetMemory))) {
    return false;
  }
  xmlInitParser();

  TileSet *tileSetList = 0;
  TileLayer *tileLayerList = 0;
  xmlDoc *doc = xmlReadFile(mapfile, 0, 0);
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
    tileMap->tileLayerList = 0;

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
          xmlFree(name);
          goto fail;
        }
        if (!gameContext->functions.loadTexture(name, value, gameContext->renderer,
                                                &gameContext->permanentMemory)) {
          xmlFree(name);
          xmlFree(value);
          goto fail;
        }
        xmlFree(name);
        xmlFree(value);
      }
    }

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
        size_t tileSetNodeNameLen = strlen(name);
        newTileSet->name = (char *) reserveMemory(&gameContext->longTimeMemory,
                                                  tileSetNodeNameLen + 1);
        strcpy(newTileSet->name, name);
        xmlFree(name);

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
            if (!gameContext->functions.loadTexture(newTileSet->name, source, gameContext->renderer,
                                                    &gameContext->permanentMemory)) {
              xmlFree(source);
              goto fail;
            }
            xmlFree(source);
            newTileSet->texture = gameContext->functions.getTexture(newTileSet->name);
          }
        }
        newTileSet->numColumns =
            newTileSet->imageWidth / (newTileSet->tileWidth + newTileSet->spacing);
      }
    }

    TileLayer* tileLayerNode = 0;
    for (xmlNode *layer = map->children; layer; layer = layer->next) {
      if (layer->type == XML_ELEMENT_NODE &&
          xmlStrcmp(layer->name, (const xmlChar *) "layer") == 0) {
        xmlNode *data = getXmlElement(layer, (const xmlChar *) "data");
        if (!data) {
          goto fail;
        }
        TileLayer *newTileLayer = (TileLayer *) reserveMemory(&gameContext->longTimeMemory,
                                                              sizeof(TileLayer));
        if (!tileLayerList) {
          tileLayerList = newTileLayer;
        }
        if (tileLayerNode) {
          tileLayerNode->next = newTileLayer;
        }
        tileLayerNode = newTileLayer;
        tileLayerNode->tileWidth = tileMap->tileWidth;
        tileLayerNode->tileHeight = tileMap->tileHeight;
        tileLayerNode->screenWidth = gameContext->screenWidth;
        tileLayerNode->screenHeight = gameContext->screenWidth;
        tileLayerNode->screenColumns = gameContext->screenWidth / tileMap->tileWidth;
        tileLayerNode->screenRows = gameContext->screenHeight / tileMap->tileHeight;
        tileLayerNode->mapWidth = tileMap->width;
        tileLayerNode->mapHeight = tileMap->height;
        tileLayerNode->tileSetList = tileSetList;
        tileLayerNode->tileGidsCount = (size_t) (tileMap->height * tileMap->width);

        xmlChar *base64Gids = xmlNodeGetContent(data);
        char *trimBase64Gids = stringTrim((char *) base64Gids);
        int32_t *gids = 0;
        if (!dataDecode(trimBase64Gids, tileLayerNode->tileGidsCount, &gids,
                        &gameContext->shortTimetMemory)) {
          xmlFree(base64Gids);
          goto fail;
        }
        xmlFree(base64Gids);

        size_t sizeofids = tileLayerNode->tileGidsCount * sizeof(int32_t);
        tileLayerNode->tileGids = (int32_t *) reserveMemory(&gameContext->longTimeMemory,
                                                            sizeofids);

        memcpy(tileLayerNode->tileGids, gids, sizeofids);
      }
    }
    tileMap->tileLayerList = tileLayerList;
    xmlNode *objectgroup = getXmlElement(map, (const xmlChar *) "objectgroup");
    if (!objectgroup) {
      goto fail;
    }
    xmlNode *object = getXmlElement(objectgroup, (const xmlChar *) "object");
    if (!object) {
      goto fail;
    }

    int objectX = 0;
    int objectY = 0;
    if (!xmlGetProp(object, (const xmlChar *) "x", &objectX)) {
      goto fail;
    }
    if (!xmlGetProp(object, (const xmlChar *) "y", &objectY)) {
      goto fail;
    }
    xmlNode *objectProperties = getXmlElement(object, (const xmlChar *) "properties");
    if (!objectProperties) {
      goto fail;
    }
    int numFrames = 0;
    int textureHeight = 0;
    int textureWidth = 0;
    int callbackId = 0;
    int animSpeed = 0;
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
          xmlFree(propertyName);
          goto fail;
        } else if (strcmp(propertyName, "numFrames") == 0) {
          numFrames = atoi(propertyValue);

        } else if (strcmp(propertyName, "textureHeight") == 0) {
          textureHeight = atoi(propertyValue);

        } else if (strcmp(propertyName, "textureWidth") == 0) {
          textureWidth = atoi(propertyValue);

        } else if (strcmp(propertyName, "callbackId") == 0) {
          callbackId = atoi(propertyValue);

        } else if (strcmp(propertyName, "animSpeed") == 0) {
          animSpeed = atoi(propertyValue);

        } else if (strcmp(propertyName, "textureId") == 0) {
          size_t propertyValueLen = strlen(propertyValue);
          textureId = (char *) reserveMemory(&gameContext->shortTimetMemory, propertyValueLen + 1);
          memcpy(textureId, propertyValue, propertyValueLen + 1);
        }
        xmlFree(propertyName);
        xmlFree(propertyValue);
      }
    }
    SDL_Texture *texture = gameContext->functions.getTexture(textureId);
    Entity *player = (Entity *) reserveMemory(&gameContext->longTimeMemory, sizeof(Entity));
    player->type = PLAYER_TYPE;
    player->position = V2D{(float) objectX, (float) objectY};
    player->bitmap = Bitmap{texture, textureWidth, textureHeight, numFrames, 1, 1};
    player->velocity = V2D{0, 0};
    player->acceleration = V2D{0, 0};

    tileMap->objectLayer = (ObjectLayer *) reserveMemory(&gameContext->longTimeMemory,
                                                         sizeof(ObjectLayer));
    tileMap->objectLayer->playerInitialPosition = player->position;
    tileMap->objectLayer->player = player;

    xmlFreeDoc(doc);

    xmlCleanupParser();

    gameContext->shortTimetMemory.usedSize = 0;

    return true;
  }
  fail:
  if (doc) {
    xmlFreeDoc(doc);
  }
  for (TileSet *tileSetNode = tileSetList; tileSetNode; tileSetNode = tileSetNode->next) {
    if (tileSetNode->name) {
      freeMemory(&gameContext->longTimeMemory, tileSetNode->name);
    }
    freeMemory(&gameContext->longTimeMemory, tileSetNode);
  }
  for (TileLayer *tileLayerNode = tileLayerList; tileLayerNode; tileLayerNode = tileLayerNode->next) {
    if (tileLayerNode->tileGids) {
      freeMemory(&gameContext->longTimeMemory, tileLayerNode->tileGids);
    }
    freeMemory(&gameContext->longTimeMemory, tileLayerNode);
  }
  if (tileMap->objectLayer && tileMap->objectLayer->player) {
    freeMemory(&gameContext->longTimeMemory, tileMap->objectLayer->player);
  }
  if (tileMap->objectLayer) {
    freeMemory(&gameContext->longTimeMemory, tileMap->objectLayer);
  }
  gameContext->shortTimetMemory.usedSize = 0;
  return false;
}
