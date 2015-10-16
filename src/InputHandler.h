//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_INPUTHANDLER_H
#define HELLO2SDL_INPUTHANDLER_H

#ifdef __APPLE__
#include <SDL2/SDL_events.h>
#else
#include <SDL_events.h>
#endif

#include "Vector2D.h"

class Game;

class InputHandler {
public:
  void update(Game* game);

  enum MouseButton {
    LEFT_MOUSE_BUTTON = 0,
    MIDDLE_MOUSE_BUTTON,
    RIGHT_MOUSE_BUTTON
  };

  bool getMouseButtonState(MouseButton mouseButton) const {
    return mouseButtonStates[mouseButton];
  }

  Vector2D getMousePosition() const {
    return mousePosition;
  }

  bool isKeyDown(SDL_Scancode key) const {
    if (keyStates) {
      return keyStates[key] == 1;
    }
    return false;
  }

  void reset();

private:
  bool mouseButtonStates[3];
  Vector2D mousePosition;
  const Uint8 *keyStates;

  void onMouseMove(SDL_Event& event);
  void onMouseButtonDown(SDL_Event& event);
  void onMouseButtonUp(SDL_Event& event);
};


#endif //HELLO2SDL_INPUTHANDLER_H
