//
// Created by Jose Gutierrez on 10/17/15.
//

#ifndef HELLO2SDL_GAME_H
#define HELLO2SDL_GAME_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include <string.h>

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

enum GameState {
  PLAY_STATE,
  MAIN_MENU_STATE,
  PAUSE_MENU_STATE,
  GAME_OVER_STATE
};

enum GameStateChange {
  NONE = 0,
  START_PLAY,
  EXIT_FROM_GAME,
  MAIN_MENU,
  RESUME_PLAY,
  PAUSE_MENU,
  GAME_OVER
};

using LoadTextureFunc = bool(const char * textureName, const char *filePath, SDL_Renderer *renderer,
                             MemoryPartition *partition);
using GetTextureFunc = SDL_Texture *(const char * textureName);
using UnloadTextureFunc = bool(const char * textureName, MemoryPartition *partition);

struct PlatformFunctions {
  LoadTextureFunc *loadTexture;
  GetTextureFunc *getTexture;
  UnloadTextureFunc *unloadTexture;
};

struct ButtonState {
  int halfTransitionCount;
  bool endedDown;
  bool wasDown;
};

struct UserInput {
  bool shouldQuit;

  int mousePositionX;
  int mousePositionY;
  int mouseWheelY;
  ButtonState mouseButtonLeft;
  ButtonState mouseButtonMiddle;
  ButtonState mouseButtonRight;

  ButtonState moveUp;
  ButtonState moveDown;
  ButtonState moveLeft;
  ButtonState moveRight;

  ButtonState actionUp;
  ButtonState actionDown;
  ButtonState actionLeft;
  ButtonState actionRight;

  ButtonState leftShoulder;
  ButtonState rightShoulder;

  ButtonState back;
  ButtonState start;
};

struct GameContext {
  char* resourcePath;
  int screenWidth;
  int screenHeight;
  bool isInitialized;
  SDL_Renderer *renderer;
  MemoryPartition permanentMemory;
  MemoryPartition longTimeMemory;
  MemoryPartition shortTimeMemory;
  PlatformFunctions functions;
  GameState currentState;
  GameStateChange stateChange;
  UserInput userInput;
};

using GameContextCallbackFunc = void(GameContext *);

void * reserveMemory(MemoryPartition *partition, size_t reserveSize);

bool freeMemory(MemoryPartition *partition, void* memory);

inline char *
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

#ifdef __cplusplus
extern "C" {
#endif

int gameUpdateAndRender(GameContext *gameContext);

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_GAME_H
