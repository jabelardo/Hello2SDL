//
// Created by Jose Gutierrez on 10/15/15.
//

#include "Game.h"
#include "InputHandler.h"

void
InputHandler::update(Game *game) {
  auto event = SDL_Event{};
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        game->quit();
        break;
      }
      case SDL_MOUSEBUTTONDOWN: {
        onMouseButtonDown(event);
        break;
      }
      case SDL_MOUSEBUTTONUP: {
        onMouseButtonUp(event);
        break;
      }
      case SDL_MOUSEMOTION: {
        onMouseMove(event);
        break;
      }
      default: {
        keyStates = SDL_GetKeyboardState(0);
        break;
      }
    }
  }
}

void
InputHandler::onMouseButtonDown(SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_LEFT) {
    mouseButtonStates[LeftMouseButton] = true;
  }
  if (event.button.button == SDL_BUTTON_MIDDLE) {
    mouseButtonStates[MiddleMouseButton] = true;
  }
  if (event.button.button == SDL_BUTTON_RIGHT) {
    mouseButtonStates[RightMouseButton] = true;
  }
}

void
InputHandler::onMouseMove(SDL_Event &event) {
  mousePosition = Vector2D{(float) event.motion.x, (float) event.motion.y};
}

void
InputHandler::onMouseButtonUp(SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_LEFT) {
    mouseButtonStates[LeftMouseButton] = false;
  }
  if (event.button.button == SDL_BUTTON_MIDDLE) {
    mouseButtonStates[MiddleMouseButton] = false;
  }
  if (event.button.button == SDL_BUTTON_RIGHT) {
    mouseButtonStates[RightMouseButton] = false;
  }
}

