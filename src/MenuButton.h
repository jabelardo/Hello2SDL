//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_MENUBUTTON_H
#define HELLO2SDL_MENUBUTTON_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "Sprite.h"
#include "UserInput.h"

struct LoaderParams;
class TextureManager;
class InputHandler;

class MenuButton {
public:

  using Callback = void(SDL_Renderer*);

  MenuButton(const LoaderParams& params, Callback* callback);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);

  void update(UserInput *inputHandler, SDL_Renderer* renderer);

  void clean();

  enum ButtonState {
    MOUSE_OUT = 0,
    MOUSE_OVER,
    CLICKED
  };

private:
  Sprite sprite;
  Callback* callback;
  bool buttonReleased;
};


#endif //HELLO2SDL_MENUBUTTON_H
