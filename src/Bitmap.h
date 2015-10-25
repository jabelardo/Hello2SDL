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
};


#endif //HELLO2SDL_BITMAP_H
