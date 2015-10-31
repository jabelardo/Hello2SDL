//
// Created by Jose Gutierrez on 10/16/15.
//

#ifndef HELLO2SDL_ANIMATEDGRAPHIC_H
#define HELLO2SDL_ANIMATEDGRAPHIC_H

#include "Bitmap.h"

struct SDL_Renderer;

struct AnimatedGraphic {
  int x;
  int y;
  Bitmap bitmap;
  int animationSpeed;
};

void drawAnimatedGraphic(AnimatedGraphic* animatedGraphic, SDL_Renderer *renderer);

void updateAnimatedGraphic(AnimatedGraphic* animatedGraphic);


#endif //HELLO2SDL_ANIMATEDGRAPHIC_H
