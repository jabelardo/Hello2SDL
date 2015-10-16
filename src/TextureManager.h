//
// Created by Jose Gutierrez on 10/14/15.
//

#ifndef HELLO2SDL_TEXTUREMANAGER_H
#define HELLO2SDL_TEXTUREMANAGER_H

#include <string>
#include <unordered_map>

#ifdef __APPLE__
#include <SDL2/SDL_render.h>
#else
#include <SDL_render.h>
#endif

#include "TextureId.h"

class TextureManager {
  std::string baseResourcePath;
  std::unordered_map<TextureId, SDL_Texture*> textureMap;

public:
  std::string getResourcePath(const std::string &filename);

  bool load(TextureId textureId, const std::string &fileName, SDL_Renderer *renderer);

  void draw(TextureId textureId, int x, int y, int width, int height, SDL_Renderer *renderer,
            SDL_RendererFlip flip = SDL_FLIP_NONE);

  void drawFrame(TextureId textureId, int x, int y, int width, int height, int currentRow,
                 int currentFrame, SDL_Renderer *renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

  void clearFromTextureMap(TextureId textureId);
};


#endif //HELLO2SDL_TEXTUREMANAGER_H
