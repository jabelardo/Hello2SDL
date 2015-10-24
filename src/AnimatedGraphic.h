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

#include "Bitmap.h"
#include "UserInput.h"

struct AnimatedGraphic {
  int x;
  int y;
  Bitmap bitmap;
  int animationSpeed;
};

void drawAnimatedGraphic(AnimatedGraphic* animatedGraphic, SDL_Renderer *renderer);

void updateAnimatedGraphic(AnimatedGraphic* animatedGraphic, UserInput *inputHandler);


#endif //HELLO2SDL_ANIMATEDGRAPHIC_H
