//
// Created by Jose Gutierrez on 10/25/15.
//

#ifndef HELLO2SDL_PLATFORM_H
#define HELLO2SDL_PLATFORM_H

#include "SharedDefinitions.h"

using gameUpdateAndRenderFunc = int(PlatformConfig *, UserInput *, GameMemory *, SDL_Renderer *);

struct GameLibrary {
  void *dl;
  time_t lastWriteTime;
  gameUpdateAndRenderFunc *gameUpdateAndRender;
  bool initialized;
};

enum ReplayStreamType {
  INPUT_REPLAY_STREAM,
  STATE_REPLAY_STREAM
};

struct PlatformReplayState {
  int stateFileHandle;
  int recordingHandle;
  int playbackHandle;

  void *stateMemoryBlock;

  char stateFilename[PLATFORM_MAX_PATH];
  char inputFilename[PLATFORM_MAX_PATH];

  ssize_t bytesWritten;
  ssize_t bytesToRead;
};

#endif //HELLO2SDL_PLATFORM_H
