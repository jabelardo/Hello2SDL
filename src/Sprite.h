//
// Created by Jose Gutierrez on 10/15/15.
//

#ifndef HELLO2SDL_SPRITE_H
#define HELLO2SDL_SPRITE_H

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "Vector2D.h"
#include "TextureId.h"

struct LoaderParams;
class TextureManager;
class InputHandler;

class Sprite {
public:
  Sprite(const LoaderParams& params);

  void draw(TextureManager* textureManager, SDL_Renderer* renderer,
            SDL_RendererFlip flip = SDL_FLIP_NONE);

  const Vector2D& getPosition() const;

  void setPosition(const Vector2D& position);

  int getCurrentFrame() const;

  void setCurrentFrame(int currentFrame);

  int getWidth() const;

  int getHeight() const;

  TextureId getTextureId() const;

  int getCurrentRow() const;

  int getTotalFrames() const;

private:
  TextureId textureId;
  Vector2D position;
  int width;
  int height;
  int currentFrame;
  int currentRow;
  int totalFrames;
};


#endif //HELLO2SDL_SPRITE_H
