#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
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
// Huge thanks to Adam Rosenfield for investigating this, and suggesting this
// workaround:
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

const auto DEFAULT_REFRESH_RATE = 60;

int
sdlGetWindowRefreshRate(SDL_Window &window) {
  auto displayIndex = SDL_GetWindowDisplayIndex(&window);

  // If we can't find the refresh rate, we'll return DEFAULT_REFRESH_RATE
  SDL_DisplayMode mode;
  if (SDL_GetDesktopDisplayMode(displayIndex, &mode) != 0) {
    return DEFAULT_REFRESH_RATE;
  }
  if (mode.refresh_rate == 0) {
    return DEFAULT_REFRESH_RATE;
  }
  return mode.refresh_rate;
}

void
sdlProcessKeyPress(ButtonState &newState, bool isDown) {
  assert(newState.endedDown != isDown);
  newState.endedDown = isDown;
  ++newState.halfTransitionCount;
}

void
sdlHandleEvent(SDL_Event &event, UserInput &userInput) {
  switch (event.type) {
    case SDL_QUIT: {
      userInput.shouldQuit = true;
      break;
    }
    case SDL_MOUSEWHEEL: {
      userInput.mouseWheelY = event.wheel.y;
      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      auto button = event.button.button;
      auto keyPressed = (event.button.state == SDL_PRESSED);
      switch (button) {
        case SDL_BUTTON_LEFT: {
          sdlProcessKeyPress(userInput.mouseButtonLeft, keyPressed);
          break;
        }
        case SDL_BUTTON_MIDDLE: {
          sdlProcessKeyPress(userInput.mouseButtonMiddle, keyPressed);
          break;
        }
        case SDL_BUTTON_RIGHT: {
          sdlProcessKeyPress(userInput.mouseButtonRight, keyPressed);
          break;
        }
      }
      break;
    }
    case SDL_MOUSEMOTION: {
      userInput.mousePositionX = event.motion.x;
      userInput.mousePositionY = event.motion.y;
      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
      auto keyCode = event.key.keysym.sym;
      auto keyPressed = (event.key.state == SDL_PRESSED);
      if (event.key.repeat == 0) {
        switch (keyCode) {
          case SDLK_w: {
            sdlProcessKeyPress(userInput.moveUp, keyPressed);
            break;
          }
          case SDLK_a: {
            sdlProcessKeyPress(userInput.moveLeft, keyPressed);
            break;
          }
          case SDLK_s: {
            sdlProcessKeyPress(userInput.moveDown, keyPressed);
            break;
          }
          case SDLK_d: {
            sdlProcessKeyPress(userInput.moveRight, keyPressed);
            break;
          }
          case SDLK_q: {
            sdlProcessKeyPress(userInput.leftShoulder, keyPressed);
            break;
          }
          case SDLK_e: {
            sdlProcessKeyPress(userInput.rightShoulder, keyPressed);
            break;
          }
          case SDLK_UP: {
            sdlProcessKeyPress(userInput.actionUp, keyPressed);
            break;
          }
          case SDLK_LEFT: {
            sdlProcessKeyPress(userInput.actionLeft, keyPressed);
            break;
          }
          case SDLK_DOWN: {
            sdlProcessKeyPress(userInput.actionDown, keyPressed);
            break;
          }
          case SDLK_RIGHT: {
            sdlProcessKeyPress(userInput.actionRight, keyPressed);
            break;
          }
          case SDLK_ESCAPE: {
            sdlProcessKeyPress(userInput.back, keyPressed);
            break;
          }
          case SDLK_SPACE: {
            sdlProcessKeyPress(userInput.start, keyPressed);
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

  auto lastCounter = SDL_GetPerformanceCounter();
  auto monitorRefreshHz = sdlGetWindowRefreshRate(*window);
  auto targetSecondsPerFrame = 1.0f / (float) monitorRefreshHz;
  auto userInput = UserInput{};
  auto gameContext = GameContext{};
  userInput.shouldQuit = false;

  while (!userInput.shouldQuit) {
    auto event = SDL_Event{};
    while (SDL_PollEvent(&event)) {
      sdlHandleEvent(event, userInput);
    }

    SDL_RenderClear(renderer);
    gameUpdateAndRender(&gameContext, &userInput, renderer);

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
