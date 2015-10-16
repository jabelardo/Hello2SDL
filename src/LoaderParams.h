//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_LOADERPARAMS_H
#define HELLO2SDL_LOADERPARAMS_H

#include "TextureId.h"

struct LoaderParams {
  TextureId textureId;
  float x;
  float y;
  int width;
  int height;
  int frames;
};

#endif //HELLO2SDL_LOADERPARAMS_H
