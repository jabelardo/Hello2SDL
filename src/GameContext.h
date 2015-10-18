//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAMECONTEXT_H
#define HELLO2SDL_GAMECONTEXT_H

#include "UserInput.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (LoadTextureFunc)(int textureId, const char* fileName, SDL_Renderer *renderer);
typedef SDL_Texture* GetTextureFunc(int textureId);
typedef bool UnloadTextureFunc(int textureId);

typedef struct PlatformFunctions {
  LoadTextureFunc* loadTexture;
  GetTextureFunc* getTexture;
  UnloadTextureFunc* unloadTexture;
} PlatformFunctions;

typedef struct MemoryPartition {
  size_t totalSize;
  size_t usedSize;
  void* base;
} MemoryPartition;

enum GameStateChange {
  NONE = 0,
  RESTART_PLAY,
  EXIT_FROM_GAME,
  SHOW_MAIN_MENU,
  RESUME_PLAY,
  PAUSE_PLAY,
  GAME_OVER
};

typedef struct GameContext {
  bool isInitialized;
  UserInput *userInput;
  SDL_Renderer *renderer;
  MemoryPartition permanentMemory;
  MemoryPartition transientMemory;
  PlatformFunctions functions;
  GameStateChange stateChange;
} GameContext;

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_GAMECONTEXT_H
