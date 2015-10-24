#ifdef __APPLE__

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#else
#include <SDL.h>
#include <SDL_image.h>
#endif

#if _MSC_VER
#include <windows.h>
#else

#include <sys/mman.h>

#endif

#include <assert.h>

#include "UserInput.h"

#include "Game.cpp"
#include "GameContext.h"

// NOTE: MAP_ANONYMOUS is not defined on Mac OS X and some other UNIX systems.
// On the vast majority of those systems, one can use MAP_ANON instead.
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#ifndef ASSETS_FOLDER
#define ASSETS_FOLDER "assets"
#endif

#define KILOBYTES(value) ((value) * 1024ul)
#define MEGABYTES(value) (KILOBYTES(value) * 1024ul)
#define GIGABYTES(value) (MEGABYTES(value) * 1024ul)

#define DEFAULT_REFRESH_RATE 60
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct TextureHashNode {
  char* name;
  SDL_Texture *texture;
  TextureHashNode *next;
};

char G_baseResourcePath[FILENAME_MAX] = {0};
char G_resourcePath[FILENAME_MAX] = {0};
GameContext G_gameContext = {};
TextureHashNode* G_textureHash[4096] = {};

uint32_t
sdbmHash(const char * str) {
  uint32_t hash = 0;
  while (int c = *str++) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }
  return hash;
}

const char *
sdlGetResourcePath(const char *filename) {
  if (G_baseResourcePath[0] == 0) {
    char *basePath = SDL_GetBasePath();
    if (!basePath) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error getting resource path: %s\n", SDL_GetError());
      return 0;
    }
    char *pos = strstr(basePath, "build");
    size_t basePathLen = (pos) ? pos - basePath : strlen(basePath);
    strncat(G_baseResourcePath, basePath, basePathLen);
    strcat(G_baseResourcePath, ASSETS_FOLDER);
    strcat(G_baseResourcePath, PATH_SEP);
    SDL_free(basePath);
  }
  if (!filename || !strlen(filename)) {
    return G_baseResourcePath;
  }
  strcpy(G_resourcePath, G_baseResourcePath);
  strcat(G_resourcePath, filename);
  return G_resourcePath;
}

bool
sdlLoadTexture(const char * textureName, const char *fileName, SDL_Renderer *renderer,
               MemoryPartition *partition) {
  uint32_t hashVal32 = sdbmHash(textureName);
  uint32_t hashPos12 = hashVal32 & 0x00000FFF;
  assert(hashPos12 < SDL_arraysize(G_textureHash));

  TextureHashNode *node = G_textureHash[hashPos12];
  TextureHashNode *parent = node;
  while (node) {
    if (strcmp(node->name, textureName) == 0) {
      return true;
    }
    parent = node;
    node = node->next;
  }
  node = (TextureHashNode *) reserveMemory(partition, sizeof(TextureHashNode));
  *node = {};
  if (parent) {
    parent->next = node;
  } else {
    G_textureHash[hashPos12] = node;
  }

  const char *resource = sdlGetResourcePath(fileName);
  if (!strlen(resource)) {
    return false;
  }
  SDL_Surface *tempSurface = IMG_Load(resource);
  if (!tempSurface) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error IMG_Load: %s\n", SDL_GetError());
    return false;
  }
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
  SDL_FreeSurface(tempSurface);
  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error SDL_CreateTextureFromSurface: %s\n",
                 SDL_GetError());
    return false;
  }
  node->texture = texture;
  size_t textureNameLen = strlen(textureName);
  node->name = (char*) reserveMemory(partition, textureNameLen + 1);
  strcpy(node->name, textureName);
  return true;
}

SDL_Texture *
sdlGetTexture(const char * textureName) {
  uint32_t hashVal32 = sdbmHash(textureName);
  uint32_t hashPos12 = hashVal32 & 0x00000FFF;
  assert(hashPos12 < SDL_arraysize(G_textureHash));

  TextureHashNode *node = G_textureHash[hashPos12];
  while (node) {
    if (strcmp(node->name, textureName) == 0) {
      return node->texture;
    }
    node = node->next;
  }
  return 0;
}

bool
sdlUnloadTexture(const char * textureName) {
//  for (currentNode = G_textureList; currentNode; currentNode = currentNode->next) {
//    if (currentNode->textureId == textureId) {
//      SDL_DestroyTexture(currentNode->texture);
//      currentNode->texture = 0;
//      return true;
//    }
//  }
  return false;
}

int
sdlGetWindowRefreshRate(SDL_Window *window) {
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

void
sdlProcessKeyPress(ButtonState *newState, bool isDown, bool wasDown) {
  if (newState->endedDown != isDown) {
    newState->endedDown = isDown;
    ++newState->halfTransitionCount;
  }
  newState->wasDown = wasDown;
}

void
sdlHandleEvent(SDL_Event *event, UserInput *userInput) {
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
          sdlProcessKeyPress(&userInput->mouseButtonLeft, isDown, wasDown);
          break;
        }
        case SDL_BUTTON_MIDDLE: {
          sdlProcessKeyPress(&userInput->mouseButtonMiddle, isDown, wasDown);
          break;
        }
        case SDL_BUTTON_RIGHT: {
          sdlProcessKeyPress(&userInput->mouseButtonRight, isDown, wasDown);
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
            sdlProcessKeyPress(&userInput->moveUp, isDown, wasDown);
            break;
          }
          case SDLK_a: {
            sdlProcessKeyPress(&userInput->moveLeft, isDown, wasDown);
            break;
          }
          case SDLK_s: {
            sdlProcessKeyPress(&userInput->moveDown, isDown, wasDown);
            break;
          }
          case SDLK_d: {
            sdlProcessKeyPress(&userInput->moveRight, isDown, wasDown);
            break;
          }
          case SDLK_q: {
            sdlProcessKeyPress(&userInput->leftShoulder, isDown, wasDown);
            break;
          }
          case SDLK_e: {
            sdlProcessKeyPress(&userInput->rightShoulder, isDown, wasDown);
            break;
          }
          case SDLK_UP: {
            sdlProcessKeyPress(&userInput->actionUp, isDown, wasDown);
            break;
          }
          case SDLK_LEFT: {
            sdlProcessKeyPress(&userInput->actionLeft, isDown, wasDown);
            break;
          }
          case SDLK_DOWN: {
            sdlProcessKeyPress(&userInput->actionDown, isDown, wasDown);
            break;
          }
          case SDLK_RIGHT: {
            sdlProcessKeyPress(&userInput->actionRight, isDown, wasDown);
            break;
          }
          case SDLK_ESCAPE: {
            sdlProcessKeyPress(&userInput->back, isDown, wasDown);
            break;
          }
          case SDLK_SPACE: {
            sdlProcessKeyPress(&userInput->start, isDown, wasDown);
            break;
          }
        }
      }
    }
  }
}

float
sdlGetSecondsElapsed(Uint64 oldCounter, Uint64 currentCounter) {
  return ((float) (currentCounter - oldCounter) /
          (float) (SDL_GetPerformanceFrequency()));
}

template <typename T>
bool isNegative(T val) {
  int result = (val > T(0)) - (val < T(0));
  return result == -1;
}

void *
reserveMemory(MemoryPartition *partition, size_t reserveSize) {
  if (partition->type == PERMANENT_MEMORY || partition->type == SHORT_TIME_MEMORY) {
    assert(reserveSize <= partition->totalSize - partition->usedSize);
    int8_t *result = (int8_t *) partition->base + partition->usedSize;
    partition->usedSize += reserveSize;
    return result;

  } else if (partition->type == LONG_TIME_MEMORY) {
    void *block = partition->base;
    while (block < (int8_t*) partition->base + partition->totalSize) {
      // NOTE:
      // each memory block is prefixed with a ssize_t value indicating the block size, the block is
      // free to use if the sign of the size header is negative
      ssize_t blockSize = *(ssize_t*) block;
      if (isNegative(blockSize)) {
        if (-1 * blockSize >= reserveSize + sizeof(ssize_t)) {
          assert((int8_t *) block + sizeof(ssize_t) + reserveSize + sizeof(ssize_t)
                 <= (int8_t *) partition->base + partition->totalSize);
          ssize_t *resultAddress = (ssize_t *) block + 1;
          partition->usedSize += reserveSize + sizeof(ssize_t);
          *(ssize_t *) ((int8_t *) resultAddress + reserveSize) = blockSize + reserveSize + sizeof(ssize_t);
          *(ssize_t *) block = reserveSize;
          return resultAddress;
        }
        block = (int8_t *) block + -1 * blockSize + sizeof(ssize_t);
      } else {
        block = (int8_t *) block + blockSize + sizeof(ssize_t);
      }
    }
    assert(false);
    return 0;
  }
  assert(false);
  return 0;
}

bool
freeMemory(MemoryPartition *partition, void* memory) {
  if (partition->type == PERMANENT_MEMORY) {
    return false;

  } else if (partition->type == SHORT_TIME_MEMORY) {
    return true;

  } else if (partition->type == LONG_TIME_MEMORY) {
    assert(memory >= partition->base);
    assert(memory <= (int8_t*) partition->base + partition->totalSize);
    ssize_t* memorySize = (ssize_t*) memory - 1;

    if (isNegative(*memorySize)) {
      return false;
    }
    partition->usedSize -= *memorySize;
    *memorySize *= -1;

    // join contiguous free blocks
    void *block = partition->base;
    ssize_t *freeBlock = (ssize_t *) 0;
    while (block < (int8_t*) partition->base + partition->totalSize) {
      ssize_t blockSize = *(ssize_t*) block;
      if (isNegative(blockSize)) {
        if (freeBlock) {
          *freeBlock += blockSize - sizeof(ssize_t);
          partition->usedSize += blockSize - sizeof(ssize_t);
        } else {
          freeBlock = (ssize_t*) block;
        }
        block = (int8_t *) block + -1 * blockSize + sizeof(ssize_t);
      } else {
        freeBlock = 0;
        block = (int8_t *) block + blockSize + sizeof(ssize_t);
      }
    }
    return true;
  }
  assert(false);
  return false;
}

int
main(int argc, char *args[]) {

  int result = SDL_Init(SDL_INIT_EVERYTHING);
  if (result != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error initializating SDL: %s\n", SDL_GetError());
    return result;
  }

  atexit(SDL_Quit);

  SDL_Window *window = SDL_CreateWindow("Chapter 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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
  void *totalReservedMemory = mmap(0, totalReservedMemorySize,  PROT_READ | PROT_WRITE,
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
  int monitorRefreshHz = sdlGetWindowRefreshRate(window);
  float targetSecondsPerFrame = 1.0f / (float) monitorRefreshHz;
  UserInput userInput = {};
  G_gameContext = GameContext{SCREEN_WIDTH, SCREEN_HEIGHT, false, &userInput, renderer,
                              permanentMemory, longTimeMemory, shortTimeMemory,
                              PlatformFunctions{sdlLoadTexture, sdlGetTexture, sdlUnloadTexture}};
  userInput.shouldQuit = false;

  while (!userInput.shouldQuit) {
    SDL_Event event = {};
    while (SDL_PollEvent(&event)) {
      sdlHandleEvent(&event, &userInput);
    }

    SDL_RenderClear(renderer);
    if (int result = gameUpdateAndRender(&G_gameContext) != 0) {
      return result;
    }

    float secondsElapsedForFrame = sdlGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter());
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

  // TODO: xmlCleanupParser();

  return 0;
}
