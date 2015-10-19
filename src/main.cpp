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

#include "GameUpdateAndRender.cpp"
#include "GameContext.h"

// NOTE: MAP_ANONYMOUS is not defined on Mac OS X and some other UNIX systems.
// On the vast majority of those systems, one can use MAP_ANON instead.
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

const auto DEFAULT_REFRESH_RATE = 60;

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

#ifndef ASSETS_FOLDER
#define ASSETS_FOLDER "assets"
#endif

#define KILOBYTES(value) ((value) * 1024LL)
#define MEGABYTES(value) (KILOBYTES(value) * 1024LL)
#define GIGABYTES(value) (MEGABYTES(value) * 1024LL)

struct TextureHashNode {
  char* name;
  SDL_Texture *texture;
  TextureHashNode *next;
};

char G_baseResourcePath[FILENAME_MAX] = {0};
char G_resourcePath[FILENAME_MAX] = {0};
GameContext G_gameContext = {};
TextureHashNode* G_textureHash[4096] = {};

// The following fastHash function is inspired from http://www.azillionmonkeys.com/qed/hash.html
// under Paul Hsieh OLD BSD license http://www.azillionmonkeys.com/qed/license-oldbsd.html
//
uint32_t 
fastHash(const char * data) {
  if (data == 0 || *data == 0) {
    return 0;
  }

  auto pos = data;
  auto tmp = uint32_t{};
  auto hash = uint32_t{};
  auto rem = 0;

  // NOTE: this code is little endian, in big endian the order is reversed 
  for (;;) {
    tmp = *(uint32_t*) pos;
    if (!(tmp & 0x000000FF)) {
      rem = 0;
      break;
    } else if (!(tmp & 0x0000FF00)) {
      rem = 1;
      break;
    } else if (!(tmp & 0x00FF0000)) {
      rem = 2;
      break;
    } else if (!(tmp & 0xFF000000)) {
      rem = 3;
      break;
    }
    hash += (tmp >> 16);
    tmp   = ((tmp & 0x0000FFFF) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    pos  += sizeof (uint32_t);
    hash  += hash >> 11;
  }
  switch (rem) {
    case 3:
      hash += (tmp >> 16);
      hash ^= hash << 16;
      hash ^= ((signed char) pos[sizeof (uint16_t)]) << 18;
      hash += hash >> 11;
      break;
    case 2:
      hash += (tmp >> 16);
      hash ^= hash << 11;
      hash += hash >> 17;
      break;
    case 1:
      hash += (signed char) *pos;
      hash ^= hash << 10;
      hash += hash >> 1;
  }
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}

const char *
sdlGetResourcePath(const char *filename) {
  if (G_baseResourcePath[0] == 0) {
    auto basePath = SDL_GetBasePath();
    if (!basePath) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error getting resource path: %s\n", SDL_GetError());
      return 0;
    }
    auto pos = strstr(basePath, "build");
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
sdlLoadTexture(const char * textureName, const char *fileName, SDL_Renderer *renderer) {
  auto hashVal32 = fastHash(textureName);
  auto hashPos12 = hashVal32 & 0x00000FFF;
  assert(hashPos12 < SDL_arraysize(G_textureHash));

  auto node = G_textureHash[hashPos12];
  auto parent = node;
  while (node) {
    if (strcmp(node->name, textureName) == 0) {
      return true;
    }
    parent = node;
    node = node->next;
  }
  node = (TextureHashNode *) reserveMemory(&G_gameContext.permanentMemory, sizeof(TextureHashNode));
  *node = {};
  if (parent) {
    parent->next = node;
  } else {
    G_textureHash[hashPos12] = node;
  }
  
  auto resource = sdlGetResourcePath(fileName);
  if (!strlen(resource)) {
    return false;
  }
  auto tempSurface = IMG_Load(resource);
  if (!tempSurface) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error IMG_Load: %s\n", SDL_GetError());
    return false;
  }
  auto texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
  SDL_FreeSurface(tempSurface);
  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error SDL_CreateTextureFromSurface: %s\n",
                 SDL_GetError());
    return false;
  }
  node->texture = texture;
  auto textureNameLen = strlen(textureName);
  node->name = (char*) reserveMemory(&G_gameContext.permanentMemory, textureNameLen + 1);
  strcpy(node->name, textureName);
  return true;
}

SDL_Texture *
sdlGetTexture(const char * textureName) {
  auto hashVal32 = fastHash(textureName);
  auto hashPos12 = hashVal32 & 0x00000FFF;
  assert(hashPos12 < SDL_arraysize(G_textureHash));

  auto node = G_textureHash[hashPos12];
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
//  for (auto currentNode = G_textureList; currentNode; currentNode = currentNode->next) {
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
  auto displayIndex = SDL_GetWindowDisplayIndex(window);

  // If we can't find the refresh rate, we'll return DEFAULT_REFRESH_RATE
  auto mode = SDL_DisplayMode{};
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
      auto button = event->button.button;
      auto isDown = (event->button.state == SDL_PRESSED);
      auto wasDown = (event->button.state == SDL_RELEASED) ||
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
      auto keyCode = event->key.keysym.sym;
      auto isDown = (event->key.state == SDL_PRESSED);
      auto wasDown = (event->key.state == SDL_RELEASED) ||
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

int
main(int argc, char *args[]) {

  auto result = SDL_Init(SDL_INIT_EVERYTHING);
  if (result != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error initializating SDL: %s\n", SDL_GetError());
    return result;
  }

  atexit(SDL_Quit);

  auto window = SDL_CreateWindow("Chapter 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                 640, 480, 0);
  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating window: %s\n", SDL_GetError());
    return 1;
  }

  auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error creating renderer: %s\n", SDL_GetError());
    return 1;
  }

  auto completeSharedMemory = MemoryPartition{MEGABYTES(32) + MEGABYTES(32)};

#if _MSC_VER
  completeSharedMemory.gameMemoryBlock = VirtualAlloc(0, completeSharedMemory.totalSize,
                                                      MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
#else
  completeSharedMemory.base = mmap(0, completeSharedMemory.totalSize,
                                   PROT_READ | PROT_WRITE,
                                   MAP_ANON | MAP_PRIVATE,
                                   -1, 0);
#endif

  auto permanentMemory = MemoryPartition{MEGABYTES(32), 0, completeSharedMemory.base};
  auto transientMemory = MemoryPartition{MEGABYTES(32), 0, (int8_t *) completeSharedMemory.base +
                                                           permanentMemory.totalSize};

  auto lastCounter = SDL_GetPerformanceCounter();
  auto monitorRefreshHz = sdlGetWindowRefreshRate(window);
  auto targetSecondsPerFrame = 1.0f / (float) monitorRefreshHz;
  auto userInput = UserInput{};
  G_gameContext = GameContext{false, &userInput, renderer, permanentMemory, transientMemory,
                              PlatformFunctions{sdlLoadTexture, sdlGetTexture, sdlUnloadTexture}};
  userInput.shouldQuit = false;

  while (!userInput.shouldQuit) {
    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) {
      sdlHandleEvent(&event, &userInput);
    }

    SDL_RenderClear(renderer);
    gameUpdateAndRender(&G_gameContext);

    auto secondsElapsedForFrame = sdlGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter());
    if (secondsElapsedForFrame < targetSecondsPerFrame) {
      auto timeToSleep = (Uint32) (targetSecondsPerFrame - secondsElapsedForFrame) * 1000;
      if (timeToSleep > 0) {
//        printf("timeToSleep %d\n", timeToSleep);
        SDL_Delay(timeToSleep);
      }
    } else {
//      printf("MISSED FRAME RATE!\n");
    }

    SDL_RenderPresent(renderer);

    auto endCounter = SDL_GetPerformanceCounter();

    lastCounter = endCounter;
  }

  // TODO: xmlCleanupParser();

  return 0;
}
