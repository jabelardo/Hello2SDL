#ifdef __APPLE__

#include <SDL2/SDL.h>

#else
#include <SDL.h>
#endif

#if _MSC_VER
#include <windows.h>
#else

#include <sys/mman.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/errno.h>

#endif

#include "SharedDefinitions.h"

// NOTE: MAP_ANONYMOUS is not defined on Mac OS X and some other UNIX systems.
// On the vast majority of those systems, one can use MAP_ANON instead.
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#define KILOBYTES(value) ((value) * 1024ul)
#define MEGABYTES(value) (KILOBYTES(value) * 1024ul)
#define GIGABYTES(value) (MEGABYTES(value) * 1024ul)

#define DEFAULT_REFRESH_RATE 60
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

/*
 * TODO:
 *  - loop game recording / playback
 */

using gameUpdateAndRenderFunc = int(PlatformConfig *, GameMemory *, UserInput *, SDL_Renderer *);

struct GameLibrary {
  void *dl;
  time_t lastWriteTime;
  gameUpdateAndRenderFunc *gameUpdateAndRender;
  bool initialized;
};

#if _MSC_VER
#else

static time_t
getLastWriteTime(const char *filename) {
  time_t lastWriteTime = 0;
  struct stat fileStat;
  if (stat(filename, &fileStat) == 0) {
    lastWriteTime = fileStat.st_mtimespec.tv_sec;
  }
  return lastWriteTime;
}

#endif

#ifdef __APPLE__

static void
loadGameLibrary(GameLibrary *gameLibrary, const char *sourceName) {
  gameLibrary->lastWriteTime = getLastWriteTime(sourceName);

  gameLibrary->dl = dlopen(sourceName, RTLD_LAZY | RTLD_GLOBAL);
  if (gameLibrary->dl) {
    gameLibrary->gameUpdateAndRender = (gameUpdateAndRenderFunc *)
        dlsym(gameLibrary->dl, "gameUpdateAndRender");

    gameLibrary->initialized = (gameLibrary->gameUpdateAndRender != 0);
  }
  if (!gameLibrary->initialized) {
    gameLibrary->gameUpdateAndRender = 0;
  }
}

static void
unloadGameLibrary(GameLibrary *gameLibrary) {
  if (gameLibrary->dl) {
    dlclose(gameLibrary->dl);
    gameLibrary->dl = 0;
  }
  gameLibrary->initialized = false;
  gameLibrary->gameUpdateAndRender = 0;
}

#endif

#ifndef ASSETS_FOLDER
#define ASSETS_FOLDER "assets"
#endif

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

static char *G_basePath = 0;
static char G_resourcePath[PLATFORM_MAX_PATH];

static int
initResourcePath() {
  if (!G_basePath) {
    G_basePath = SDL_GetBasePath();
  }
  if (!G_basePath) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error getting resource path: %s\n", SDL_GetError());
    return -1;
  }
  char *pos = strstr(G_basePath, "build");
  size_t basePathLen = (pos) ? pos - G_basePath : strlen(G_basePath);
  size_t resourcePathLen = basePathLen + strlen(ASSETS_FOLDER) + 2;
  if (resourcePathLen > SDL_arraysize(G_resourcePath)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Resource path lenght %d > %d\n", resourcePathLen,
                 SDL_arraysize(G_resourcePath));
    return -1;
  }
  G_resourcePath[0] = 0;
  strncat(G_resourcePath, G_basePath, basePathLen);
  strcat(G_resourcePath, ASSETS_FOLDER);
  strcat(G_resourcePath, PATH_SEP);
  return 0;
}

static int
getWindowRefreshRate(SDL_Window *window) {
  int displayIndex = SDL_GetWindowDisplayIndex(window);

  // If we can't find the refresh rate, we'll return DEFAULT_REFRESH_RATE
  SDL_DisplayMode mode = {};
  if (SDL_GetDesktopDisplayMode(displayIndex, &mode) != 0) {
    return DEFAULT_REFRESH_RATE;
  }
  if (mode.refresh_rate == 0) {
    return DEFAULT_REFRESH_RATE;
  }
  return mode.refresh_rate;
}

static float
getSecondsElapsed(Uint64 oldCounter, Uint64 currentCounter) {
  return ((float) (currentCounter - oldCounter) /
          (float) (SDL_GetPerformanceFrequency()));
}

struct PlatformReplayBuffer {
  int fileHandle;
//  int memoryMap;
  char filename[PLATFORM_MAX_PATH];
  void *memoryBlock;
};

struct PlatformReplayState {

  int recordingHandle;
  int recordingIndex;

  int playbackHandle;
  int playingIndex;

  GameMemory *gameMemory;

  PlatformReplayBuffer replayBuffers[4];
};

static int
getInputFileLocation(char *destName, size_t destSize, int slotIndex, bool isInputStream = true) {
  return snprintf(destName, destSize, "%sloop_edit_%d_%s.mem", G_basePath, slotIndex,
                  isInputStream ? "input" : "state");
}

static void
setupPlatformReplayBuffers(PlatformReplayState *state) {

  size_t totalSize = state->gameMemory->permanentMemory.totalSize +
                     state->gameMemory->longTimeMemory.totalSize;

  for (int replayBufferIdx = 0; replayBufferIdx < SDL_arraysize(state->replayBuffers);
       ++replayBufferIdx) {
    PlatformReplayBuffer *replayBuffer = &state->replayBuffers[replayBufferIdx];

    getInputFileLocation(replayBuffer->filename, SDL_arraysize(replayBuffer->filename),
                         replayBufferIdx, false);

    replayBuffer->fileHandle = open(replayBuffer->filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (replayBuffer->fileHandle != -1) {
      replayBuffer->memoryBlock = mmap(0, totalSize,
                                       PROT_READ | PROT_WRITE,
                                       MAP_PRIVATE,
                                       replayBuffer->fileHandle,
                                       0);
      if (replayBuffer->memoryBlock != MAP_FAILED) {
        fstore_t fstore = {};
        fstore.fst_flags = F_ALLOCATECONTIG;
        fstore.fst_posmode = F_PEOFPOSMODE;
        fstore.fst_offset = 0;
        fstore.fst_length = totalSize;

        int Result = fcntl(replayBuffer->fileHandle, F_PREALLOCATE, &fstore);
        if (Result != -1) {
          Result = ftruncate(replayBuffer->fileHandle, totalSize);

          if (Result != 0) {
            printf("ftruncate error on replayBuffer[%d]: %d: %s\n",
                   replayBufferIdx, errno, strerror(errno));
          }
        } else {
          printf("fcntl error on replayBuffer[%d]: %d: %s\n",
                 replayBufferIdx, errno, strerror(errno));
        }
      } else {
        printf("mmap error on replayBuffer[%d]: %d  %s",
               replayBufferIdx, errno, strerror(errno));
      }
    } else {
      printf("Error creating replayBuffer[%d] file %s: %d : %s\n",
             replayBufferIdx, replayBuffer->filename, errno, strerror(errno));
    }
  }
}

static PlatformReplayBuffer *
getReplayBuffer(PlatformReplayState *state, int index) {
  assert(index < SDL_arraysize(state->replayBuffers));
  PlatformReplayBuffer *result = &state->replayBuffers[index];
  return result;
}

static void
beginInputPlayback(int inputPlayingIndex, PlatformReplayState *state) {
  printf("beginning input playback\n");

  PlatformReplayBuffer *replayBuffer = getReplayBuffer(state, inputPlayingIndex);

  if (replayBuffer->memoryBlock) {
    state->playingIndex = inputPlayingIndex;
    state->playbackHandle = open(replayBuffer->filename, O_RDONLY);

    memcpy(state->gameMemory->permanentMemory.base,
           replayBuffer->memoryBlock,
           state->gameMemory->permanentMemory.totalSize);

    memcpy(state->gameMemory->longTimeMemory.base,
           (int8_t *) replayBuffer->memoryBlock + state->gameMemory->permanentMemory.totalSize,
           state->gameMemory->longTimeMemory.totalSize);
  }
}

static void
endInputPlayback(PlatformReplayState *state) {
  close(state->playbackHandle);
  state->playingIndex = -1;
  printf("ended input playback\n");
}

static void
beginInputRecording(int inputRecordingIndex, PlatformReplayState *state) {
  printf("beginning recording input\n");

  PlatformReplayBuffer *replayBuffer = getReplayBuffer(state, inputRecordingIndex);

  if (state->playingIndex > -1) {
    printf("...first stopping input playback\n");
    endInputPlayback(state);
  }

  if (replayBuffer->memoryBlock) {
    state->recordingIndex = inputRecordingIndex;

    getInputFileLocation(replayBuffer->filename, SDL_arraysize(replayBuffer->filename),
                         inputRecordingIndex);
    state->recordingHandle = open(replayBuffer->filename, O_WRONLY | O_CREAT, 0644);

    memcpy(replayBuffer->memoryBlock,
           state->gameMemory->permanentMemory.base,
           state->gameMemory->permanentMemory.totalSize);

    memcpy((int8_t *) replayBuffer->memoryBlock + state->gameMemory->permanentMemory.totalSize,
           state->gameMemory->longTimeMemory.base,
           state->gameMemory->longTimeMemory.totalSize);
  }
}

static void
endInputRecording(PlatformReplayState *state) {
  close(state->recordingHandle);
  state->recordingIndex = -1;
  printf("ended recording input\n");
}

static void
recordInput(PlatformReplayState *state, UserInput *userInput) {

  ssize_t bytesWritten = write(state->recordingHandle, userInput, sizeof(UserInput));

  if (bytesWritten != sizeof(UserInput)) {
    printf("write error recording input: %d: %s\n", errno, strerror(errno));
  }
}

static void
playbackInput(PlatformReplayState *state, UserInput *userInput) {

  ssize_t bytesRead = read(state->playbackHandle, userInput, sizeof(UserInput));

  printf("bytesRead: %zd\n", bytesRead);

  if (bytesRead == 0) {
    // NOTE(casey): We've hit the end of the stream, go back to the beginning
    int playingIndex = state->playingIndex;
    endInputPlayback(state);
    beginInputPlayback(playingIndex, state);

    bytesRead = read(state->playbackHandle, userInput, sizeof(UserInput));

    if (bytesRead != sizeof(UserInput)) {
      printf("read error rewinding playback input: %d: %s\n", errno, strerror(errno));

    } else {
      printf("rewinding playback...\n");
    }
  } else if (bytesRead < 0) {
    int t = 0;
  }
}

static void
processKeyPress(ButtonState *newState, bool isDown, bool wasDown) {
  if (newState->endedDown != isDown) {
    newState->endedDown = isDown;
    ++newState->halfTransitionCount;
  }
  newState->wasDown = wasDown;
}

static void
handleEvent(SDL_Event *event, UserInput *userInput, PlatformReplayState *state) {
  switch (event->type) {
    case SDL_QUIT: {
      userInput->shouldQuit = true;
      break;
    }
    case SDL_MOUSEWHEEL: {
      userInput->mouseWheelY = event->wheel.y;
      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      Uint8 button = event->button.button;
      bool isDown = (event->button.state == SDL_PRESSED);
      bool wasDown = (event->button.state == SDL_RELEASED) ||
                     (event->button.clicks != 0);
      switch (button) {
        case SDL_BUTTON_LEFT: {
          processKeyPress(&userInput->mouseButtonLeft, isDown, wasDown);
          break;
        }
        case SDL_BUTTON_MIDDLE: {
          processKeyPress(&userInput->mouseButtonMiddle, isDown, wasDown);
          break;
        }
        case SDL_BUTTON_RIGHT: {
          processKeyPress(&userInput->mouseButtonRight, isDown, wasDown);
          break;
        }
      }
      break;
    }
    case SDL_MOUSEMOTION: {
      userInput->mousePositionX = event->motion.x;
      userInput->mousePositionY = event->motion.y;
      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      SDL_Keycode keyCode = event->key.keysym.sym;
      bool isDown = (event->key.state == SDL_PRESSED);
      bool wasDown = (event->key.state == SDL_RELEASED) ||
                     (event->key.repeat != 0);
      if (event->key.repeat == 0) {
        switch (keyCode) {
          case SDLK_w: {
            processKeyPress(&userInput->moveUp, isDown, wasDown);
            break;
          }
          case SDLK_a: {
            processKeyPress(&userInput->moveLeft, isDown, wasDown);
            break;
          }
          case SDLK_s: {
            processKeyPress(&userInput->moveDown, isDown, wasDown);
            break;
          }
          case SDLK_d: {
            processKeyPress(&userInput->moveRight, isDown, wasDown);
            break;
          }
          case SDLK_q: {
            processKeyPress(&userInput->leftShoulder, isDown, wasDown);
            break;
          }
          case SDLK_e: {
            processKeyPress(&userInput->rightShoulder, isDown, wasDown);
            break;
          }
          case SDLK_UP: {
            processKeyPress(&userInput->actionUp, isDown, wasDown);
            break;
          }
          case SDLK_LEFT: {
            processKeyPress(&userInput->actionLeft, isDown, wasDown);
            break;
          }
          case SDLK_DOWN: {
            processKeyPress(&userInput->actionDown, isDown, wasDown);
            break;
          }
          case SDLK_RIGHT: {
            processKeyPress(&userInput->actionRight, isDown, wasDown);
            break;
          }
          case SDLK_ESCAPE: {
            processKeyPress(&userInput->back, isDown, wasDown);
            break;
          }
          case SDLK_SPACE: {
            processKeyPress(&userInput->start, isDown, wasDown);
            break;
          }
#ifdef BUILD_INTERNAL
          case SDLK_p: {
            if (isDown) {
              userInput->globalPause = !userInput->globalPause;
              break;
            }
          }
          case SDLK_l: {
            if (isDown) {
              if (state->playingIndex == -1) {
                if (state->recordingIndex == -1) {
                  beginInputRecording(0, state);
                } else {
                  endInputRecording(state);
                  beginInputPlayback(0, state);
                }
              } else {
                endInputPlayback(state);
              }
            }
          }
#endif
        }
      }
    }
  }
}

int
main(int argc, char *args[]) {

  int result = SDL_Init(SDL_INIT_EVERYTHING);
  if (result != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error initializating SDL: %s\n", SDL_GetError());
    return result;
  }

  atexit(SDL_Quit);

  initResourcePath();

  SDL_Window *window = SDL_CreateWindow("Chapter 1", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating window: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating renderer: %s\n", SDL_GetError());
    return 1;
  }

  size_t totalReservedMemorySize = MEGABYTES(32) + MEGABYTES(32) + MEGABYTES(32);

#if _MSC_VER
  void *totalReservedMemory = VirtualAlloc(0, totalReservedMemorySize, MEM_RESERVE | MEM_COMMIT,
                                           PAGE_READWRITE);
#else
  void *totalReservedMemory = mmap(0, totalReservedMemorySize, PROT_READ | PROT_WRITE,
                                   MAP_ANON | MAP_PRIVATE, -1, 0);
#endif

  MemoryPartition permanentMemory = {PERMANENT_MEMORY, MEGABYTES(32), 0, totalReservedMemory};

  MemoryPartition longTimeMemory = {LONG_TIME_MEMORY, MEGABYTES(32), 0,
                                    (int8_t *) permanentMemory.base + permanentMemory.totalSize};

  // init longTimeMemory
  *(ssize_t *) longTimeMemory.base = -longTimeMemory.totalSize - sizeof(ssize_t);
  longTimeMemory.usedSize = sizeof(ssize_t);

  MemoryPartition shortTimeMemory = {SHORT_TIME_MEMORY, MEGABYTES(32), 0,
                                     (int8_t *) longTimeMemory.base + longTimeMemory.totalSize};

  Uint64 lastCounter = SDL_GetPerformanceCounter();
  int monitorRefreshHz = getWindowRefreshRate(window);
  float targetSecondsPerFrame = 1.0f / (float) monitorRefreshHz;

  PlatformConfig platformConfig = {G_resourcePath, SCREEN_WIDTH, SCREEN_HEIGHT};
  GameMemory gameMemory = {permanentMemory, longTimeMemory, shortTimeMemory};
  PlatformReplayState state = {-1, -1, -1, -1, &gameMemory};
  UserInput userInput = {};
  GameLibrary gameLibrary = {};

  setupPlatformReplayBuffers(&state);

  while (!userInput.shouldQuit) {

#ifdef BUILD_INTERNAL
    time_t gameLibraryLastWriteTime = getLastWriteTime("libGame.dylib");
    if (gameLibraryLastWriteTime > gameLibrary.lastWriteTime) {
      unloadGameLibrary(&gameLibrary);
      loadGameLibrary(&gameLibrary, "libGame.dylib");
    }
    if (!gameLibrary.initialized) {
      // TODO SDL_LogError
      return 1;
    }
#endif

    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      handleEvent(&event, &userInput, &state);
    }
#ifdef BUILD_INTERNAL
    if (userInput.globalPause) {
      continue;
    }
#endif
    if (state.recordingIndex > -1) {
      recordInput(&state, &userInput);
    }
    if (state.playingIndex > -1) {
      playbackInput(&state, &userInput);
    }

    SDL_RenderClear(renderer);

#ifdef BUILD_INTERNAL
    if ((result = gameLibrary.gameUpdateAndRender(
        &platformConfig, &gameMemory, &userInput, renderer) != 0)) {
      return result;
    }
#else
    if ((result = gameUpdateAndRender(&platformConfig, &gameMemory, &userInput, renderer) != 0)) {
      return result;
    }
#endif

    float secondsElapsedForFrame = getSecondsElapsed(lastCounter, SDL_GetPerformanceCounter());
    if (secondsElapsedForFrame < targetSecondsPerFrame) {
      Uint32 timeToSleep = (Uint32) (targetSecondsPerFrame - secondsElapsedForFrame) * 1000;
      if (timeToSleep > 0) {
//        printf("timeToSleep %d\n", timeToSleep);
        SDL_Delay(timeToSleep);
      }
    } else {
//      printf("MISSED FRAME RATE!\n");
    }

    SDL_RenderPresent(renderer);

    Uint64 endCounter = SDL_GetPerformanceCounter();

    lastCounter = endCounter;
  }
  return 0;
}
