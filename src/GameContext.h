//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAMECONTEXT_H
#define HELLO2SDL_GAMECONTEXT_H

#include <stdint.h>
#include <new>
#include <assert.h>

#include "UserInput.h"

typedef bool (LoadTextureFunc)(int textureId, const char* fileName, SDL_Renderer *renderer);
typedef SDL_Texture* GetTextureFunc(int textureId);
typedef bool UnloadTextureFunc(int textureId);

struct PlatformFunctions {
  LoadTextureFunc* loadTexture;
  GetTextureFunc* getTexture;
  UnloadTextureFunc* unloadTexture;
};

struct MemoryPartition {
  size_t totalSize;
  size_t usedSize;
  void* base;
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

inline int8_t *
reserveMemory(MemoryPartition *partition, size_t memorySize) {
  assert(memorySize < partition->totalSize - partition->usedSize);
  auto result = (int8_t *) partition->base + partition->usedSize;
  partition->usedSize += memorySize;
  return result;
}

#define PLACEMENT_NEW(MEMORY, TYPE) new(reserveMemory(MEMORY, sizeof(TYPE)))

#endif //HELLO2SDL_GAMECONTEXT_H
