//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_ANIMATEDGRAPHIC_H
#define HELLO2SDL_ANIMATEDGRAPHIC_H

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


class AnimatedGraphic {
public:
  AnimatedGraphic(const LoaderParams& params, int animationSpeed);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer);

  void update(UserInput *inputHandler);

  void clean();

private:
  Sprite sprite;
  int animationSpeed;
};


#endif //HELLO2SDL_ANIMATEDGRAPHIC_H
