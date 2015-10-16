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
    mouseButtonStates[LEFT_MOUSE_BUTTON] = true;
  }
  if (event.button.button == SDL_BUTTON_MIDDLE) {
    mouseButtonStates[MIDDLE_MOUSE_BUTTON] = true;
  }
  if (event.button.button == SDL_BUTTON_RIGHT) {
    mouseButtonStates[RIGHT_MOUSE_BUTTON] = true;
  }
}

void
InputHandler::onMouseMove(SDL_Event &event) {
  mousePosition = Vector2D{(float) event.motion.x, (float) event.motion.y};
}

void
InputHandler::onMouseButtonUp(SDL_Event &event) {
  if (event.button.button == SDL_BUTTON_LEFT) {
    mouseButtonStates[LEFT_MOUSE_BUTTON] = false;
  }
  if (event.button.button == SDL_BUTTON_MIDDLE) {
    mouseButtonStates[MIDDLE_MOUSE_BUTTON] = false;
  }
  if (event.button.button == SDL_BUTTON_RIGHT) {
    mouseButtonStates[RIGHT_MOUSE_BUTTON] = false;
  }
}

void InputHandler::reset() {
  for (auto& mouseButtonState : mouseButtonStates) {
    mouseButtonState = false;
  }
}
