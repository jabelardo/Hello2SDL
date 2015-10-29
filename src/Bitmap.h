//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_BITMAP_H
#define HELLO2SDL_BITMAP_H

struct SDL_Texture;

struct Bitmap {
  SDL_Texture *texture;
  int width;
  int height;
  int totalFrames;
  int currentFrame;
  int currentRow;
  double angle;
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t alpha;
};


#endif //HELLO2SDL_BITMAP_H
