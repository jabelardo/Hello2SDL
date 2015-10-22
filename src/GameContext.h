//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAMECONTEXT_H
#define HELLO2SDL_GAMECONTEXT_H

#include <new>
#include <assert.h>

#include "UserInput.h"

enum MemoryPartitionType {
  PERMANENT_MEMORY = 0,
  LONG_TIME_MEMORY,
  SHORT_TIME_MEMORY,
};

struct MemoryPartition {
  MemoryPartitionType type;
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

using LoadTextureFunc = bool(const char * textureName, const char *fileName, SDL_Renderer *renderer,
                             MemoryPartition *partition);
using GetTextureFunc = SDL_Texture *(const char * textureName);
using UnloadTextureFunc = bool(const char * textureName);

struct PlatformFunctions {
  LoadTextureFunc *loadTexture;
  GetTextureFunc *getTexture;
  UnloadTextureFunc *unloadTexture;
};

struct GameContext {
  int screenWidth;
  int screenHeight;
  bool isInitialized;
  UserInput *userInput;
  SDL_Renderer *renderer;
  MemoryPartition permanentMemory;
  MemoryPartition longTimeMemory;
  MemoryPartition shortTimetMemory;
  PlatformFunctions functions;
  GameStateChange stateChange;
};

using GameContextCallbackFunc = void(GameContext *);

void * reserveMemory(MemoryPartition *partition, size_t reserveSize);

#define PLACEMENT_NEW(MEMORY, TYPE) new(reserveMemory(MEMORY, sizeof(TYPE)))

bool freeMemory(MemoryPartition *partition, void* memory);

#endif //HELLO2SDL_GAMECONTEXT_H
