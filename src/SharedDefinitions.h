//
// Created by Jose Gutierrez on 10/25/15.
//

#ifndef HELLO2SDL_SHAREDDEFINITIONS_H
#define HELLO2SDL_SHAREDDEFINITIONS_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "MemoryPartition.h"

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
};

#ifdef __cplusplus
extern "C" {
#endif

int gameUpdateAndRender(PlatformConfig *platformConfig, GameMemory* gameMemory,
                        UserInput* userInput, SDL_Renderer *renderer);

#ifdef __cplusplus
}
#endif

#endif //HELLO2SDL_SHAREDDEFINITIONS_H
