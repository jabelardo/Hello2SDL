//
// Created by Jose Gutierrez on 10/24/15.
//

#ifdef __APPLE__
#include <SDL2/SDL_log.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_Mixer/SDL_Mixer.h>
#else
#include <SDL_log.h>
#include <SDL_image.h>
#include <SDL_Mixer.h>
#endif

#include <assert.h>

#include "AssetsManagement.h"
#include "MemoryPartition.h"
#include "SharedDefinitions.h"

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

static bool
loadAsset(const char *assetName, AssetType type, const char *fileName, const char* resourcePath,
          GameContext *gameContext, GameMemory* gameMemory, SDL_Renderer *renderer = 0) {

  assert(type != TEXTURE_ASSET || renderer);

  uint32_t hashVal32 = sdbmHash(assetName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->assetHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->assetHash));

  AssetHashNode *node = gameContext->assetHash[hashPos12];
  AssetHashNode *parent = node;
  while (node) {
    if ((strcmp(node->name, assetName) == 0) && (node->type == type)) {
      return true;
    }
    parent = node;
    node = node->next;
  }
  if (gameContext->freeAssetHashNodes) {
    node = gameContext->freeAssetHashNodes;
    gameContext->freeAssetHashNodes = gameContext->freeAssetHashNodes->next;
    node->next = 0;
  } else {
    node = (AssetHashNode *) reserveMemory(&gameMemory->longTimeMemory, sizeof(AssetHashNode));
    *node = {};
  }
  if (parent) {
    parent->next = node;
  } else {
    gameContext->assetHash[hashPos12] = node;
  }
  char *filePath = stringConcat(resourcePath, fileName, &gameMemory->shortTimeMemory);
  void* data = 0;

  if (type == TEXTURE_ASSET) {
    SDL_Surface *tempSurface = IMG_Load(filePath);
    if (!tempSurface) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error IMG_Load: %s\n", SDL_GetError());

      // return node to gameContext->freeTextureHashNodes
      node->next = gameContext->freeAssetHashNodes;
      gameContext->freeAssetHashNodes = node;
      return false;
    }
    data = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    if (!data) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error SDL_CreateTextureFromSurface: %s\n",
                   SDL_GetError());
    }
  } else if (type == MUSIC_ASSET) {
    data = Mix_LoadMUS(filePath);
    if (data == 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error Mix_LoadMUS: %s\n", SDL_GetError());
    }
  } else if (type == SOUND_ASSET) {
    data = Mix_LoadWAV(filePath);
    if (data == 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error Mix_LoadWAV: %s\n", SDL_GetError());
    }
  }
  if (!data) {
    // return node to gameContext->freeTextureHashNodes
    node->next = gameContext->freeAssetHashNodes;
    gameContext->freeAssetHashNodes = node;
    return false;
  }
  node->type = type;
  node->data = data;
  size_t assetNameLen = strlen(assetName);
  if (node->name) {
    size_t oldNodeNameLen = strlen(node->name);
    if (oldNodeNameLen != assetNameLen) {
      freeMemory(&gameMemory->longTimeMemory, node->name);
      node->name = 0;
    }
  }
  if (!node->name) {
    node->name = (char *) reserveMemory(&gameMemory->longTimeMemory, assetNameLen + 1);
  }
  strcpy(node->name, assetName);
  return true;
}


static bool
unloadAsset(const char *assetName, AssetType type, GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(assetName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->assetHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->assetHash));

  AssetHashNode **node = &gameContext->assetHash[hashPos12];
  while (node) {
    if ((strcmp((*node)->name, assetName) == 0) && ((*node)->type == type)) {
      if ((*node)->type == TEXTURE_ASSET) {
        SDL_DestroyTexture((SDL_Texture *) (*node)->data);

      } else if ((*node)->type == MUSIC_ASSET) {
        Mix_FreeMusic((Mix_Music *) (*node)->data);

      } else  if ((*node)->type == SOUND_ASSET) {
        Mix_FreeChunk((Mix_Chunk *) (*node)->data);
      }
      AssetHashNode *toRemove = *node;
      *node = (*node)->next;

      toRemove->next = gameContext->freeAssetHashNodes;
      gameContext->freeAssetHashNodes = toRemove;
      return true;

    } else {
      node = &(*node)->next;
    }
  }
  return false;
}

static void *
getAsset(const char *assetName, AssetType type, GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(assetName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->assetHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->assetHash));

  AssetHashNode *node = gameContext->assetHash[hashPos12];
  while (node) {
    if (strcmp(node->name, assetName) == 0 && node->type == type) {
      return node->data;
    }
    node = node->next;
  }
  return 0;
}

bool
loadTexture(const char *textureName, const char *fileName, const char* resourcePath, SDL_Renderer *renderer,
            GameContext *gameContext, GameMemory* gameMemory) {
  return loadAsset(textureName, TEXTURE_ASSET, fileName, resourcePath,gameContext, gameMemory, renderer);
}

bool
loadSound(const char *soundName,const char *fileName, const char* resourcePath,  GameContext *gameContext,
          GameMemory* gameMemory) {
  return loadAsset(soundName, SOUND_ASSET, fileName, resourcePath,gameContext, gameMemory);
}

bool
loadMusic(const char *soundName,const char *fileName, const char* resourcePath, GameContext *gameContext,
          GameMemory* gameMemory) {
  return loadAsset(soundName, MUSIC_ASSET, fileName, resourcePath,gameContext, gameMemory);
}

bool
unloadTexture(const char *textureName, GameContext *gameContext) {
  return unloadAsset(textureName, TEXTURE_ASSET, gameContext);
}

bool
unloadSound(const char *soundName, GameContext *gameContext) {
  return unloadAsset(soundName, SOUND_ASSET, gameContext);
}

bool
unloadMusic(const char *soundName, GameContext *gameContext) {
  return unloadAsset(soundName, MUSIC_ASSET, gameContext);

}

SDL_Texture *
getTexture(const char *textureName, GameContext *gameContext) {
  return  (SDL_Texture *) getAsset(textureName, TEXTURE_ASSET, gameContext);
}

void
playSound(const char *soundName, GameContext *gameContext, int loops) {
  Mix_Chunk * chunk = (Mix_Chunk *) getAsset(soundName, SOUND_ASSET, gameContext);
  if (chunk) {
    Mix_PlayChannel(-1, chunk, loops);
  }
}

void
playMusic(const char *soundName, GameContext *gameContext, int loops) {
  Mix_Music * music = (Mix_Music *) getAsset(soundName, MUSIC_ASSET, gameContext);
  if (music) {
    Mix_PlayMusic(music, loops);
  }
}