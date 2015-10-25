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
#include "SharedDefinitions.h"

#endif

//#include "Game.h"

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

time_t
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

static char G_resourcePath[1024];

static int
initResourcePath() {
#ifdef _WIN32
  const char* PATH_SEP = "\\";
#else
  const char *PATH_SEP = "/";
#endif

  char *basePath = SDL_GetBasePath();
  if (!basePath) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error getting resource path: %s\n", SDL_GetError());
    return -1;
  }
  char *pos = strstr(basePath, "build");
  size_t basePathLen = (pos) ? pos - basePath : strlen(basePath);
  size_t resourcePathLen = basePathLen + strlen(ASSETS_FOLDER) + 2;
  if (resourcePathLen > SDL_arraysize(G_resourcePath)) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Resource path lenght %d > %d\n", resourcePathLen,
                 SDL_arraysize(G_resourcePath));
    return -1;
  }
  G_resourcePath[0] = 0;
  strncat(G_resourcePath, basePath, basePathLen);
  strcat(G_resourcePath, ASSETS_FOLDER);
  strcat(G_resourcePath, PATH_SEP);
  SDL_free(basePath);
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

static void
processKeyPress(ButtonState *newState, bool isDown, bool wasDown) {
  if (newState->endedDown != isDown) {
    newState->endedDown = isDown;
    ++newState->halfTransitionCount;
  }
  newState->wasDown = wasDown;
}

static void
handleEvent(SDL_Event *event, UserInput *userInput) {
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
        }
      }
    }
  }
}

static float
getSecondsElapsed(Uint64 oldCounter, Uint64 currentCounter) {
  return ((float) (currentCounter - oldCounter) /
          (float) (SDL_GetPerformanceFrequency()));
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
  UserInput userInput = {};
  GameLibrary gameLibrary = {};

  while (!userInput.shouldQuit) {

#ifdef BUILD_INTERNAL
    time_t gameLibraryLastWriteTime =  getLastWriteTime("libGame.dylib");
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
      handleEvent(&event, &userInput);
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
