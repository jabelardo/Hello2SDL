//
// Created by Jose Gutierrez on 10/25/15.
//

#ifndef HELLO2SDL_SHAREDDEFINITIONS_H
#define HELLO2SDL_SHAREDDEFINITIONS_H

#include "MemoryPartition.h"

#define PLATFORM_MAX_PATH 1024

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

#ifdef BUILD_INTERNAL
  bool globalPause;
#endif
};

struct PlatformFunctions {
};

struct GameMemory {
  MemoryPartition permanentMemory;
  MemoryPartition longTimeMemory;
  MemoryPartition shortTimeMemory;
  PlatformFunctions functions;
  bool isInitialized;
};

struct PlatformConfig {
  char *resourcePath;
  int screenWidth;
  int screenHeight;
  float dtPerFrame;
};

struct SDL_Renderer;

#ifdef __cplusplus
extern "C" {
#endif

int gameUpdateAndRender(PlatformConfig *platformConfig, UserInput* userInput,
                        GameMemory* gameMemory, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_SHAREDDEFINITIONS_H
