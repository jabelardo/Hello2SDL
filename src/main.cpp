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
#include <unistd.h>
#include <sys/errno.h>

#endif

#include <assert.h>
#include <time.h>
#include "Platform.h"

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

static int
getInputFileLocation(char *destName, size_t destSize, ReplayStreamType type) {
  return snprintf(destName, destSize, "%sloop_edit_%s.mem", G_basePath,
                  (type == INPUT_REPLAY_STREAM) ? "input" : "state");
}

static void
setupPlatformReplayBuffers(PlatformReplayState *state, GameMemory *gameMemory) {

  size_t totalSize = gameMemory->permanentMemory.totalSize + gameMemory->longTimeMemory.totalSize;

  getInputFileLocation(state->stateFilename, SDL_arraysize(state->stateFilename),
                       STATE_REPLAY_STREAM);

  state->stateFileHandle = open(state->stateFilename,
                                O_RDWR | O_CREAT | O_TRUNC,
                                0644);

  if (state->stateFileHandle != -1) {
    state->stateMemoryBlock = mmap(0, totalSize,
                                   PROT_READ | PROT_WRITE,
                                   MAP_PRIVATE,
                                   state->stateFileHandle,
                                   0);
    if (state->stateMemoryBlock != MAP_FAILED) {
      fstore_t fstore = {};
      fstore.fst_flags = F_ALLOCATECONTIG;
      fstore.fst_posmode = F_PEOFPOSMODE;
      fstore.fst_offset = 0;
      fstore.fst_length = totalSize;

      int result = fcntl(state->stateFileHandle, F_PREALLOCATE, &fstore);
      if (result != -1) {
        result = ftruncate(state->stateFileHandle, totalSize);

        if (result != 0) {
          printf("ftruncate error on replayBuffer: %d: %s\n", errno, strerror(errno));
        }
      } else {
        printf("fcntl error on replayBuffer: %d: %s\n", errno, strerror(errno));
      }
    } else {
      printf("mmap error on replayBuffer: %d  %s", errno, strerror(errno));
    }
  } else {
    printf("Error creating replayBuffer file %s: %d : %s\n",
           state->stateFilename, errno, strerror(errno));
  }
}

static void
beginInputRecording(PlatformReplayState *state, GameMemory *gameMemory) {
  // printf("beginning recording input\n");

  assert(state->playbackHandle < 0);

  if (state->stateMemoryBlock) {
    getInputFileLocation(state->inputFilename, SDL_arraysize(state->inputFilename),
                         INPUT_REPLAY_STREAM);
    state->recordingHandle = open(state->inputFilename, O_WRONLY | O_CREAT, 0644);
    state->bytesWritten = 0;
    state->bytesToRead = 0;

    int8_t *writePtr = (int8_t *) state->stateMemoryBlock;

    memcpy(writePtr, &gameMemory->permanentMemory.usedSize, sizeof(gameMemory->permanentMemory.usedSize));

    writePtr += sizeof(gameMemory->permanentMemory.usedSize);

    memcpy(writePtr, gameMemory->permanentMemory.base, gameMemory->permanentMemory.usedSize);

    writePtr += gameMemory->permanentMemory.usedSize;

    memcpy(writePtr, gameMemory->longTimeMemory.base, gameMemory->longTimeMemory.totalSize);
  }
}

static void
endInputRecording(PlatformReplayState *state) {
  close(state->recordingHandle);
  state->recordingHandle = -1;
  // printf("ended recording input\n");
}

static void
beginInputPlayback(PlatformReplayState *state, GameMemory *gameMemory) {
  // printf("beginning input playback\n");

  if (state->stateMemoryBlock) {
    state->playbackHandle = open(state->inputFilename, O_RDONLY);
    state->bytesToRead = state->bytesWritten;

    int8_t *readPtr = (int8_t *) state->stateMemoryBlock;

    memcpy(&gameMemory->permanentMemory.usedSize, readPtr, sizeof(gameMemory->permanentMemory.usedSize));

    readPtr += sizeof(gameMemory->permanentMemory.usedSize);

    memcpy(gameMemory->permanentMemory.base, readPtr, gameMemory->permanentMemory.usedSize);

    readPtr += gameMemory->permanentMemory.usedSize;

    memcpy(gameMemory->longTimeMemory.base, readPtr, gameMemory->longTimeMemory.totalSize);
  }
}

static void
endInputPlayback(PlatformReplayState *state) {
  close(state->playbackHandle);
  state->playbackHandle = -1;
  // printf("ended input playback\n");
}

static void
recordInput(UserInput *userInput, PlatformReplayState *state) {

  ssize_t bytesWritten = write(state->recordingHandle, userInput, sizeof(UserInput));

  state->bytesWritten += bytesWritten;

  if (bytesWritten != sizeof(UserInput)) {
    printf("write error recording input: %d: %s\n", errno, strerror(errno));
  }
}

static void
playbackInput(UserInput *userInput, PlatformReplayState *state, GameMemory *gameMemory) {

  ssize_t bytesRead = 0;

  if (state->bytesToRead > 0) {
    bytesRead = read(state->playbackHandle, userInput, sizeof(UserInput));
  }

  if (bytesRead == 0) {
    // We've hit the end of the stream, go back to the beginning
    endInputPlayback(state);
    beginInputPlayback(state, gameMemory);

    bytesRead = read(state->playbackHandle, userInput, sizeof(UserInput));

    if (bytesRead != sizeof(UserInput)) {
      printf("read error rewinding playback input: %d: %s\n", errno, strerror(errno));

    } else {
      // printf("rewinding playback...\n");
    }
  } else if (bytesRead > 0) {
    state->bytesToRead -= bytesRead;
    assert(state->bytesToRead > -1);
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
handleEvent(SDL_Event *event, UserInput *userInput, PlatformReplayState *state,
            GameMemory* gameMemory) {
  switch (event->type) {
    case SDL_QUIT: {
      userInput->shouldQuit = true;
      break;
    }
    case SDL_WINDOWEVENT:  {
      switch (event->window.event)  {
        case SDL_WINDOWEVENT_FOCUS_GAINED: {
          if (userInput->globalPause) {
            userInput->globalPause = false;
          }
          break;
        }
        case SDL_WINDOWEVENT_FOCUS_LOST: {
          if (!userInput->globalPause) {
            userInput->globalPause = true;
          }
          break;
        }
      }
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
      bool wasDown = (event->button.state == SDL_RELEASED) || (event->button.clicks != 0);
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
      bool wasDown = (event->key.state == SDL_RELEASED) || (event->key.repeat != 0);
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
              if (state->playbackHandle == -1) {
                if (state->recordingHandle == -1) {
                  beginInputRecording(state, gameMemory);
                } else {
                  endInputRecording(state);
                  beginInputPlayback(state, gameMemory);
                }
              } else {
                endInputPlayback(state);
              }
            }
            break;
          }
#endif
        }
      }
    }
  }
}

int
main(int argc, char *args[]) {

  srand((unsigned int) time(0));

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

  PlatformConfig platformConfig = {G_resourcePath, SCREEN_WIDTH, SCREEN_HEIGHT, targetSecondsPerFrame};
  GameMemory gameMemory = {permanentMemory, longTimeMemory, shortTimeMemory};
  PlatformReplayState state = {-1, -1, -1};
  UserInput userInput = {};
  GameLibrary gameLibrary = {};

  setupPlatformReplayBuffers(&state, &gameMemory);

  while (!userInput.shouldQuit) {

#ifdef BUILD_INTERNAL
    time_t gameLibraryLastWriteTime = getLastWriteTime("libGame.dylib");
    if (gameLibraryLastWriteTime > gameLibrary.lastWriteTime) {
      unloadGameLibrary(&gameLibrary);
      loadGameLibrary(&gameLibrary, "libGame.dylib");
      printf("loadGameLibrary\n");
    }
    if (!gameLibrary.initialized) {
      // TODO SDL_LogError
      return 1;
    }
#endif

    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      handleEvent(&event, &userInput, &state, &gameMemory);
    }
#ifdef BUILD_INTERNAL
    if (userInput.globalPause) {
      continue;
    }
    if (state.recordingHandle > -1) {
      recordInput(&userInput, &state);
    }
    if (state.playbackHandle > -1) {
      playbackInput(&userInput, &state, &gameMemory);
    }
#endif

    SDL_RenderClear(renderer);

#ifdef BUILD_INTERNAL
    if ((result = gameLibrary.gameUpdateAndRender(
        &platformConfig, &userInput, &gameMemory, renderer) != 0)) {
      return result;
    }
#else
    if ((result = gameUpdateAndRender(&platformConfig, &userInput, &gameMemory, renderer) != 0)) {
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
      printf("MISSED FRAME RATE!\n");
    }

    SDL_RenderPresent(renderer);

    Uint64 endCounter = SDL_GetPerformanceCounter();

    lastCounter = endCounter;
  }
  return 0;
}
