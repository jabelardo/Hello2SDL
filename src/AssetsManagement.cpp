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

bool
loadTexture(const char *textureName, const char *fileName, const char* resourcePath,
            SDL_Renderer *renderer, GameContext *gameContext, GameMemory* gameMemory) {
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
    node = (TextureHashNode *) reserveMemory(&gameMemory->longTimeMemory, sizeof(TextureHashNode));
    *node = {};
  }
  if (parent) {
    parent->next = node;
  } else {
    gameContext->textureHash[hashPos12] = node;
  }
  char *filePath = stringConcat(resourcePath, fileName, &gameMemory->shortTimeMemory);
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
      freeMemory(&gameMemory->longTimeMemory, node->name);
      node->name = 0;
    }
  }
  if (!node->name) {
    node->name = (char *) reserveMemory(&gameMemory->longTimeMemory, textureNameLen + 1);
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

  TextureHashNode **node = &gameContext->textureHash[hashPos12];
  while (node) {
    if (strcmp((*node)->name, textureName) == 0) {
      SDL_DestroyTexture((*node)->texture);
      TextureHashNode *toRemove = *node;
      *node = (*node)->next;

      toRemove->next = gameContext->freeTextureHashNodes;
      gameContext->freeTextureHashNodes = toRemove;
      return true;

    } else {
      node = &(*node)->next;
    }
  }
  return false;
}

bool
loadSound(const char *soundName, SoundType type, const char *fileName, const char* resourcePath,
          GameContext *gameContext, GameMemory* gameMemory) {

  uint32_t hashVal32 = sdbmHash(soundName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->soundHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->soundHash));

  SoundHashNode *node = gameContext->soundHash[hashPos12];
  SoundHashNode *parent = node;
  while (node) {
    if ((strcmp(node->name, soundName) == 0) && (node->type == type)) {
      return true;
    }
    parent = node;
    node = node->next;
  }
  if (gameContext->freeSoundHashNodes) {
    node = gameContext->freeSoundHashNodes;
    gameContext->freeSoundHashNodes = gameContext->freeSoundHashNodes->next;
    node->next = 0;
  } else {
    node = (SoundHashNode *) reserveMemory(&gameMemory->longTimeMemory, sizeof(SoundHashNode));
    *node = {};
  }
  if (parent) {
    parent->next = node;
  } else {
    gameContext->soundHash[hashPos12] = node;
  }
  char *filePath = stringConcat(resourcePath, fileName, &gameMemory->shortTimeMemory);
  void* data = 0;
  if (type == SOUND_MUSIC) {
    data = Mix_LoadMUS(filePath);
    if (data == 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error Mix_LoadMUS: %s\n", SDL_GetError());
      return false;
    }
  } else if (type == SOUND_SFX) {
    data = Mix_LoadWAV(filePath);
    if (data == 0) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error Mix_LoadWAV: %s\n", SDL_GetError());
      return false;
    }
  }
  node->type = type;
  node->data = data;
  size_t soundNameLen = strlen(soundName);
  if (node->name) {
    size_t oldNodeNameLen = strlen(node->name);
    if (oldNodeNameLen != soundNameLen) {
      freeMemory(&gameMemory->longTimeMemory, node->name);
      node->name = 0;
    }
  }
  if (!node->name) {
    node->name = (char *) reserveMemory(&gameMemory->longTimeMemory, soundNameLen + 1);
  }
  strcpy(node->name, soundName);
  return true;
}


static void *
getSound(const char *textureName, SoundType type, GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(textureName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->soundHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->soundHash));

  SoundHashNode *node = gameContext->soundHash[hashPos12];
  while (node) {
    if (strcmp(node->name, textureName) == 0 && node->type == type) {
      return node->data;
    }
    node = node->next;
  }
  return 0;
}

void
playSound(const char *soundName, GameContext *gameContext, int loops) {
  Mix_Chunk * chunk = (Mix_Chunk *) getSound(soundName, SOUND_SFX, gameContext);
  if (chunk) {
    Mix_PlayChannel(-1, chunk, loops);
  }
}

void
playMusic(const char *soundName, GameContext *gameContext, int loops) {
  Mix_Music * music = (Mix_Music *) getSound(soundName, SOUND_MUSIC, gameContext);
  if (music) {
    Mix_PlayMusic(music, loops);
  }
}

bool
unloadSound(const char *soundName, SoundType type, GameContext *gameContext) {
  uint32_t hashVal32 = sdbmHash(soundName);
  uint32_t hashPos12 = hashVal32 & (uint32_t) SDL_arraysize(gameContext->soundHash) - 1;
  assert(hashPos12 < SDL_arraysize(gameContext->soundHash));

  SoundHashNode **node = &gameContext->soundHash[hashPos12];
  while (node) {
    if ((strcmp((*node)->name, soundName) == 0) && ((*node)->type == type)) {
      if ((*node)->type == SOUND_MUSIC) {
        Mix_FreeMusic((Mix_Music *) (*node)->data);
      } else {
        Mix_FreeChunk((Mix_Chunk *) (*node)->data);
      }
      SoundHashNode *toRemove = *node;
      *node = (*node)->next;

      toRemove->next = gameContext->freeSoundHashNodes;
      gameContext->freeSoundHashNodes = toRemove;
      return true;

    } else {
      node = &(*node)->next;
    }
  }
  return false;
}