//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAMECONTEXT_H
#define HELLO2SDL_GAMECONTEXT_H

#include <new>
#include <assert.h>

#include "UserInput.h"

using LoadTextureFunc = bool(const char * textureName, const char *fileName, SDL_Renderer *renderer);
using GetTextureFunc = SDL_Texture *(const char * textureName);
using UnloadTextureFunc = bool(const char * textureName);

struct PlatformFunctions {
  LoadTextureFunc *loadTexture;
  GetTextureFunc *getTexture;
  UnloadTextureFunc *unloadTexture;
};

struct MemoryPartition {
  size_t totalSize;
  size_t usedSize;
  void *base;
};

enum GameStateChange {
  NONE = 0,
  RESTART_PLAY,
  EXIT_FROM_GAME,
  SHOW_MAIN_MENU,
  RESUME_PLAY,
  PAUSE_PLAY,
  GAME_OVER
};

struct GameContext {
  bool isInitialized;
  UserInput *userInput;
  SDL_Renderer *renderer;
  MemoryPartition permanentMemory;
  MemoryPartition transientMemory;
  PlatformFunctions functions;
  GameStateChange stateChange;
};

using GameContextCallbackFunc = void(GameContext *);

inline void *
reserveMemory(MemoryPartition *partition, size_t memorySize) {
  assert(memorySize <= partition->totalSize - partition->usedSize);
  auto result = (int8_t *) partition->base + partition->usedSize;
  partition->usedSize += memorySize;
  return result;
}

#define PLACEMENT_NEW(MEMORY, TYPE) new(reserveMemory(MEMORY, sizeof(TYPE)))

#endif //HELLO2SDL_GAMECONTEXT_H