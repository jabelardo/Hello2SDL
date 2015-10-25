//
// Created by Jose Gutierrez on 10/24/15.
//

#ifdef __APPLE__
#include <SDL2/SDL_log.h>
#include <SDL2_image/SDL_image.h>
#else
#include <SDL_log.h>
#include <SDL_image.h>
#endif

#include <assert.h>

#include "TextureStorage.h"

char *
stringConcat(const char * str1, const char * str2, MemoryPartition* memoryPartition) {
  char *result = (char *) reserveMemory(memoryPartition, strlen(str1) + strlen(str2) + 1);
  if (!result) {
    return 0;
  }
  result[0] = 0;
  strcpy(result, str1);
  strcat(result, str2);
  return result;
}

static uint32_t
sdbmHash(const char *str) {
  uint32_t hash = 0;
  while (int c = *str++) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
  return hash;
}

bool
loadTexture(const char *textureName, const char *filename, SDL_Renderer *renderer,
            GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(textureName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->textureHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->textureHash));

  TextureHashNode *node = gameContext->textureHash[hashPos12];
  TextureHashNode *parent = node;
  while (node) {
    if (strcmp(node->name, textureName) == 0) {
      return true;
    }
    parent = node;
    node = node->next;
  }
  if (gameContext->freeTextureHashNodes) {
    node = gameContext->freeTextureHashNodes;
    gameContext->freeTextureHashNodes = gameContext->freeTextureHashNodes->next;
    node->next = 0;
  } else {
    node = (TextureHashNode *) reserveMemory(&gameContext->longTimeMemory, sizeof(TextureHashNode));
    *node = {};
  }
  if (parent) {
    parent->next = node;
  } else {
    gameContext->textureHash[hashPos12] = node;
  }
  char *filePath = stringConcat(gameContext->resourcePath, filename, &gameContext->shortTimeMemory);
  SDL_Surface *tempSurface = IMG_Load(filePath);
  if (!tempSurface) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error IMG_Load: %s\n", SDL_GetError());

    // return node to gameContext->freeTextureHashNodes
    node->next = gameContext->freeTextureHashNodes;
    gameContext->freeTextureHashNodes = node;
    return false;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
  SDL_FreeSurface(tempSurface);
  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error SDL_CreateTextureFromSurface: %s\n",
                 SDL_GetError());
    // return node to gameContext->freeTextureHashNodes
    node->next = gameContext->freeTextureHashNodes;
    gameContext->freeTextureHashNodes = node;
    return false;
  }
  node->texture = texture;
  size_t textureNameLen = strlen(textureName);
  if (node->name) {
    size_t oldNodeNameLen = strlen(node->name);
    if (oldNodeNameLen != textureNameLen) {
      freeMemory(&gameContext->longTimeMemory, node->name);
      node->name = 0;
    }
  }
  if (!node->name) {
    node->name = (char *) reserveMemory(&gameContext->longTimeMemory, textureNameLen + 1);
  }
  strcpy(node->name, textureName);
  return true;
}

SDL_Texture *
getTexture(const char *textureName, GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(textureName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->textureHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->textureHash));

  TextureHashNode *node = gameContext->textureHash[hashPos12];
  while (node) {
    if (strcmp(node->name, textureName) == 0) {
      return node->texture;
    }
    node = node->next;
  }
  return 0;
}

bool
unloadTexture(const char *textureName, GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(textureName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->textureHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->textureHash));

  TextureHashNode *node = gameContext->textureHash[hashPos12];
  TextureHashNode *parent = 0;
  while (node) {
    if (strcmp(node->name, textureName) == 0) {
      parent->next = node->next;
      SDL_DestroyTexture(node->texture);
      node->next = (gameContext->freeTextureHashNodes)? gameContext->freeTextureHashNodes : 0;
      gameContext->freeTextureHashNodes = node;
      return true;
    }
    parent = node;
    node = node->next;
  }
  return false;
}
