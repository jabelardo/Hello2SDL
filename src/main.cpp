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
#include <unordered_map>

#include "UserInput.h"

#include "GameUpdateAndRender.cpp"
#include "GameContext.h"

// NOTE: MAP_ANONYMOUS is not defined on Mac OS X and some other UNIX systems.
// On the vast majority of those systems, one can use MAP_ANON instead.
// Huge thanks to Adam Rosenfield for investigating this, and suggesting this
// workaround:
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
#define TERABYTES(value) (GIGABYTES(value) * 1024LL)

std::string G_baseResourcePath;

std::unordered_map<int, SDL_Texture*> G_textureMap;

std::string
sdlGetResourcePath(const std::string &filename) {
  if (G_baseResourcePath.empty()) {
    auto basePathRaw = SDL_GetBasePath();
    if (!basePathRaw) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error getting resource path: %s\n", SDL_GetError());
      return "";
    }
    auto basePath = std::string{basePathRaw};
    SDL_free(basePathRaw);
    auto pos = basePath.find("build");
    if (pos == std::string::npos) {
      G_baseResourcePath = basePath + ASSETS_FOLDER + PATH_SEP;
    } else {
      G_baseResourcePath = basePath.substr(0, pos) + ASSETS_FOLDER + PATH_SEP;
    }
  }
  auto result = G_baseResourcePath;
  if (!filename.empty()) {
    result += filename;
  }
  return result;
}

bool
sdlLoadTexture(int textureId, const char* fileName, SDL_Renderer *renderer) {
  assert(G_textureMap.count(textureId) == 0);
  auto resource = sdlGetResourcePath(fileName);
  if (resource.empty()) {
    return false;
  }
  auto tempSurface = IMG_Load(resource.c_str());
  if (!tempSurface) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error IMG_Load: %s\n", SDL_GetError());
    return false;
  }
  auto texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
  SDL_FreeSurface(tempSurface);
  if (!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
    return false;
  }
  auto result = G_textureMap.insert({textureId, texture});
  return result.second;
}

SDL_Texture*
sdlGetTexture(int textureId) {
  assert(G_textureMap.count(textureId) == 1);
  auto found = G_textureMap.find(textureId);
  if (found == G_textureMap.end()) {
    return 0;
  }
  return found->second;
}

bool
sdlUnloadTexture(int textureId) {
  auto texture = sdlGetTexture(textureId);
  if (!texture) {
    return false;
  }
  SDL_DestroyTexture(texture);
  G_textureMap.erase(textureId);
  return true;
}

int
sdlGetWindowRefreshRate(SDL_Window* window) {
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
sdlProcessKeyPress(ButtonState* newState, bool isDown, bool wasDown) {
  if (newState->endedDown != isDown) {
    newState->endedDown = isDown;
    ++newState->halfTransitionCount;
  }
  newState->wasDown = wasDown;
}

void
sdlHandleEvent(SDL_Event* event, UserInput* userInput) {
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
//#if BUILD_INTERNAL
//  void *baseAddress = (void *) GIGABYTES(1);
//#else
//  void *baseAddress = (void *) 0;
//#endif
//  auto gameMemory = GameMemory{};


  auto lastCounter = SDL_GetPerformanceCounter();
  auto monitorRefreshHz = sdlGetWindowRefreshRate(window);
  auto targetSecondsPerFrame = 1.0f / (float) monitorRefreshHz;
  auto userInput = UserInput{};
  auto gameContext = GameContext{false, &userInput, renderer,
                                 PlatformFunctions{sdlLoadTexture, sdlGetTexture, sdlUnloadTexture}};
  userInput.shouldQuit = false;

  while (!userInput.shouldQuit) {
    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) {
      sdlHandleEvent(&event, &userInput);
    }

    SDL_RenderClear(renderer);
    gameUpdateAndRender(&gameContext);

    auto secondsElapsedForFrame = sdlGetSecondsElapsed(lastCounter, SDL_GetPerformanceCounter());
    if (secondsElapsedForFrame < targetSecondsPerFrame) {
      auto timeToSleep = (Uint32) (targetSecondsPerFrame - secondsElapsedForFrame) * 1000;
      if (timeToSleep > 0) {
//        printf("timeToSleep %d\n", timeToSleep);
        SDL_Delay(timeToSleep);
      }
    } else {
      printf("MISSED FRAME RATE!\n");
    }

    SDL_RenderPresent(renderer);

    auto endCounter = SDL_GetPerformanceCounter();

    lastCounter = endCounter;
  }

  return 0;
}
